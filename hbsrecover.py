'''
Inspired from https://github.com/Mikiya83/hbs_decipher
'''
from enum import Enum, auto
from struct import unpack
from pathlib import Path
from getpass import getpass
from logging import getLogger, basicConfig, INFO, DEBUG
from argparse import ArgumentParser

from Crypto.Hash import MD5
from Crypto.Cipher import AES

basicConfig(format="(%(name)s)[%(levelname)7s]: %(message)s", level=DEBUG)
app_log = getLogger('hbsrecover')
app_log.setLevel(INFO)

class HBSFileVersion(Enum):
    '''
    '''
    V1 = auto()
    V2 = auto()
    OPENSSL = auto()
    UNKNOWN = auto()

class HBSFileCompression(Enum):
    '''Compression
    '''
    FLAT = auto()
    COMPRESSED = auto()

AES_BLOCK_SZ = 16
READ_BLOCK_SZ = 256 * AES_BLOCK_SZ  # 4096 bytes
HBS_HEADER_MAP = {
    # v1 prefix
    HBSFileVersion.V1: b'__QCS__',
    # v2 prefix
    # compression:
    #   true if prefix followed by \x01
    #   false if prefix followed by \x00
    HBSFileVersion.V2: b'K\xca\x94r^\x83\x1c1\x01',
    # openssl prefix
    HBSFileVersion.OPENSSL: b'Salted__',
}
HBS_SIGN_MAX_SZ = 16

class HBSFileHeaderV2:
    '''
    '''
    MAGIC_SZ = 8
    CKEY_SZ = 32
    SALT_SZ = 16
    DATA_SZ = 8
    SIZE = sum([
        MAGIC_SZ,
        CKEY_SZ,
        SALT_SZ,
        DATA_SZ
    ])

    def __init__(self, hdr_bytes, passphrase):
        '''Constructor
        '''
        key = passphrase * (1 + 32 // len(passphrase))
        cipher = AES.new(key[:32], mode=AES.MODE_ECB)
        plain_hdr_bytes = cipher.decrypt(hdr_bytes)
        # struct is magic [8] + ckey[32] + salt [16] + size [8]
        offset = 0
        self._magic = plain_hdr_bytes[offset:offset + HBSFileHeaderV2.MAGIC_SZ]
        offset += HBSFileHeaderV2.MAGIC_SZ
        self._ckey = plain_hdr_bytes[offset:offset + HBSFileHeaderV2.CKEY_SZ]
        offset += HBSFileHeaderV2.CKEY_SZ
        self._salt = plain_hdr_bytes[offset:offset + HBSFileHeaderV2.SALT_SZ]
        offset += HBSFileHeaderV2.SALT_SZ
        self._size = plain_hdr_bytes[offset:offset + HBSFileHeaderV2.DATA_SZ]

    def __str__(self):
        text = [
            "HBSFileHeaderV2(",
            f"  magic={self.magic}",
            f"  ckey={self.ckey}",
            f"  salt={self.salt}",
            f"  plain_size={self.plain_size}",
            ")"
        ]
        return "\n".join(text)

    @property
    def magic(self):
        return self._magic

    @property
    def ckey(self):
        return self._ckey

    @property
    def salt(self):
        return self._salt

    @property
    def plain_size(self):
        return unpack('>Q', self._size)[0]

def hbs_v1_decrypt(passphrase, infile, outfile, compression):
    '''HBS V1 decryption algorithm
    '''
    raise NotImplementedError("Not implemented, ask me if needed.")

def hbs_v2_decrypt(passphrase, infile, outfile, compression):
    '''HBS V2 decryption algorithm
    '''
    with infile.open('rb') as ifp:
        # decrypt header
        app_log.debug("decrypting header...")
        hdr_size = HBS_SIGN_MAX_SZ + HBSFileHeaderV2.SIZE
        hdr_bytes = ifp.read(hdr_size)
        hdr = HBSFileHeaderV2(hdr_bytes[HBS_SIGN_MAX_SZ:], passphrase)
        app_log.debug(hdr)
        # perform size checks
        st = infile.stat()
        enc_sz = st.st_size - hdr_size
        if hdr.plain_size >= enc_sz:
            app_log.warning("inconsistent size in deciphered header: corrupted header or bad passphrase.")
            return False
        # decrypt file data
        app_log.debug("decrypting data...")
        cipher = AES.new(hdr.ckey, AES.MODE_CBC, hdr.salt)
        with outfile.open('wb') as ofp:
            remain = enc_sz
            while True:
                remain -= READ_BLOCK_SZ
                enc_block = ifp.read(READ_BLOCK_SZ)
                if not enc_block:
                    break
                plain_block = cipher.decrypt(enc_block)
                # last block must be processed to remove PKCS#7 padding
                if remain <= 0:
                    pad_sz = plain_block[-1]
                    app_log.debug(f"removing {pad_sz} bytes of padding.")
                    plain_block = plain_block[:-pad_sz]
                ofp.write(plain_block)
        return True

def openssl_decrypt(passphrase, infile, outfile, compression):
    '''OpenSSL decryption algorithm
    '''
    raise NotImplementedError("Not implemented, ask me if needed.")

HBS_DECRYPT_MAP = {
    HBSFileVersion.V1: hbs_v1_decrypt,
    HBSFileVersion.V2: hbs_v2_decrypt,
    HBSFileVersion.OPENSSL: openssl_decrypt,
}

class HBSEncryptedFile:
    '''Represent a QNAP HBS encrypted file
    '''

    def __init__(self, filepath):
        '''Constructor
        '''
        if not filepath.is_file():
            raise RuntimeError("HBSEncryptedFile path should be a regular file!")
        self._filepath = filepath

    @property
    def filepath(self):
        '''Path to encrypted file
        '''
        return self._filepath

    @property
    def version(self):
        if not getattr(self, '_version', None):
            setattr(self, '_version', HBSFileVersion.UNKNOWN)
            with self._filepath.open('rb') as ifp:
                hdr_bytes = ifp.read(HBS_SIGN_MAX_SZ)
                for hdr_vers, hdr_sign in HBS_HEADER_MAP.items():
                    if hdr_bytes.startswith(hdr_sign):
                        setattr(self, '_version', hdr_vers)
        return getattr(self, '_version')

    @property
    def compression(self):
        if not getattr(self, '_compression', None):
            setattr(self, '_compression', HBSFileCompression.FLAT)
            if self.version == HBSFileVersion.V2:
                with self._filepath.open('rb') as ifp:
                    hdr_bytes = ifp.read(len(HBS_HEADER_MAP[HBSFileVersion.V2])+1)
                    if hdr_bytes.endswith(b'\x01'):
                        setattr(self, '_compression', HBSFileCompression.COMPRESSED)
        return getattr(self, '_compression')

    def decrypt(self, passphrase, outdir):
        '''Write decrypted version of the file to outdir
        '''
        app_log.info(f"decrypting {self.filepath} (version={self.version.name}, compression={self.compression.name}) ...")
        return HBS_DECRYPT_MAP[self.version](passphrase,
                                             self.filepath,
                                             outdir.joinpath(self.filepath.name),
                                             self.compression)

def parse_args():
    '''Parse command line arguments
    '''
    parser = ArgumentParser(description="Hybrid Backup Sync Recovery Tool")
    parser.add_argument('--debug', '-d', action='store_true', help="Enable debugging output.")
    parser.add_argument('inpath', type=Path, help="Input can be a file or a directory")
    parser.add_argument('outdir', type=Path, help="Output directory where decrypted files will be written")
    args = parser.parse_args()
    if args.debug:
        app_log.setLevel(DEBUG)
        app_log.debug(args)
    while True:
        passphrase = getpass("Enter encryption passphrase (cannot be empty): ")
        if not passphrase:
            app_log.error("Passphrase cannot be empty!")
            continue
        break
    args.passphrase = passphrase.encode()
    return args

def decrypt_dir(passphrase, inpath, outdir):
    '''Decrypt files recursively
    '''
    newoutdir = outdir.joinpath(inpath.name)
    newoutdir.mkdir(parents=True, exist_ok=True)
    for filepath in inpath.iterdir():
        if filepath.is_dir():
            decrypt_dir(passphrase, filepath, newoutdir)
        else:
            HBSEncryptedFile(filepath).decrypt(passphrase, newoutdir)

def app():
    '''Entry point
    '''
    args = parse_args()
    try:
        args.outdir.mkdir(parents=True, exist_ok=True)
        if args.inpath.is_dir():
            decrypt_dir(args.passphrase, args.inpath, args.outdir)
        else:
            HBSEncryptedFile(args.inpath).decrypt(args.passphrase, args.outdir)
    except:
        app_log.exception("Exception caught! Details below.")

if __name__ == '__main__':
        app()
