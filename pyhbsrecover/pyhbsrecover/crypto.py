'''
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from Crypto.Hash import MD5
from Crypto.Cipher import AES
from .logging import app_log

# ------------------------------------------------------------------------------
# GLOBALS
# ------------------------------------------------------------------------------
AES_STRENGTH = 256  # 256 bits
AES_BLOCK_SZ = 16  # 16 bytes
AES_CHUNK_SZ = AES_BLOCK_SZ * 256  # 4096 bytes
AES_IV_SZ = AES_BLOCK_SZ
AES_KEY_SZ = AES_STRENGTH // 8
# ------------------------------------------------------------------------------
# FUNCTIONS
# ------------------------------------------------------------------------------
def openssl_pbkdf(key_sz, iv_sz, salt, passphrase, iterations):
    """"""
    app_log.info("performing openssl pbkdf operation...")
    key = bytearray([0] * key_sz)
    iv = bytearray([0] * iv_sz)
    i = 0
    niv = iv_sz
    nkey = key_sz
    addmd = 0
    iv_ix = 0
    key_ix = 0
    md_buf = None
    if not passphrase:
        return bytes(key), bytes(iv)
    while nkey > 0 or niv > 0:
        mda = MD5.new()
        if addmd > 0:
            mda.update(md_buf)
        addmd += 1
        mda.update(passphrase)
        if salt:
            mda.update(salt[0:8])
        md_buf = bytearray(mda.digest())
        for i in range(1, iterations):
            mda = MD5.new()
            mda.update(md_buf)
            md_buf = bytearray(mda.digest())
        i = 0
        while nkey > 0 and i < len(md_buf):
            key[key_ix] = md_buf[i]
            key_ix += 1
            nkey -= 1
            i += 1
        while niv > 0 and i < len(md_buf):
            iv[iv_ix] = md_buf[i]
            iv_ix += 1
            niv -= 1
            i += 1
    return bytes(key), bytes(iv)


def get_aes_ecb_cipher(key):
    """Create a new AES ECB cipher"""
    return AES.new(key, AES.MODE_ECB)


def get_aes_cbc_cipher(key, iv):
    """Create a new AES CBC cipher"""
    return AES.new(key, AES.MODE_CBC, iv)


def aes_cbc_decrypt_data(key, iv, ifp, size, outfile):
    """Decrypt size data read from ifp to outfile using key and iv"""
    app_log.info("decrypting data...")
    cipher = get_aes_cbc_cipher(key, iv)
    with outfile.open('wb') as ofp:
        remain = size
        while True:
            remain -= AES_CHUNK_SZ
            enc_ck = ifp.read(AES_CHUNK_SZ)
            if not enc_ck:
                break
            plain_ck = cipher.decrypt(enc_ck)
            # last block must be processed to remove PKCS#7 padding
            if remain <= 0:
                pad_sz = plain_ck[-1]
                plain_ck = plain_ck[:-pad_sz]
                app_log.debug("removed %d bytes of padding.", pad_sz)
            ofp.write(plain_ck)
