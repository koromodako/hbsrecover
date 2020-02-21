'''HBSEncryptedFileV2 definition
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from pprint import pformat
from Crypto.Hash import MD5
from Crypto.Cipher import AES
from ..meta import BaseHBSEncryptedFile
from ..const import CHUNK_SZ
from ..logging import app_log
# ------------------------------------------------------------------------------
# CLASSES
# ------------------------------------------------------------------------------
class HBSEncryptedFileV2(BaseHBSEncryptedFile):
    MAGIC = b'K\xca\x94r^\x83\x1c1'
    VERSION = 2

    HDR_SPECS = {
        'magic': '8c',
        'ckey': '32c',
        'salt': '16c',
        'data_sz': '>Q'
    }

    def _parse_header(self, ifp):
        '''Parse header data
        '''
        size = 16
        data = ifp.read(size)
        return size, data[9] != 0

    def _parse_subheader(self, passphrase, ifp):
        '''Parse subheader data
        '''
        hdr_size = self._header_size(HDR_SPECS)
        ciphered = ifp.read(hdr_size)
        key = passphrase * (1 + 32 // len(passphrase))
        cipher = AES.new(key[:32], mode=AES.MODE_ECB)
        deciphered = cipher.decrypt(ciphered)
        hdr = self._unpack_header(HDR_SPECS, deciphered)
        # postprocess header
        hdr['magic'] = b''.join(hdr['magic'])
        hdr['ckey'] = b''.join(hdr['ckey'])
        hdr['salt'] = b''.join(hdr['salt'])
        hdr['data_sz'] = hdr['data_sz'][0]
        app_log.debug(pformat(hdr))
        return hdr_size, hdr

    def _decrypt_data(self, ckey, salt, ifp, outfile):
        '''
        '''
        cipher = AES.new(ckey, AES.MODE_CBC, salt)
        with outfile.open('wb') as ofp:
            remain = st.st_size - offset
            while True:
                remain -= CHUNK_SZ
                enc_ck = ifp.read(CHUNK_SZ)
                if not enc_ck:
                    break
                plain_ck = cipher.decrypt(enc_ck)
                # last block must be processed to remove PKCS#7 padding
                if remain <= 0:
                    pad_sz = plain_ck[-1]
                    app_log.debug(f"removing {pad_sz} bytes of padding.")
                    plain_ck = plain_ck[:-pad_sz]
                ofp.write(plain_ck)

    def decrypt(self, passphrase, outdir):
        '''Write decrypted version of the file to outdir
        '''
        with self.filepath.open('rb') as ifp:
            offset = 0
            # read file header to detect compression
            app_log.debug("parsing header...")
            size, compression = self._parse_header(ifp)
            offset += size
            app_log.debug("compression: %s", compression)
            if compression:
                raise NotImplementedError("HBSEncryptedFileV2 compression not supported.")
            # decrypt subheader
            app_log.debug("parsing subheader...")
            size, hdr = self._parse_subheader(passphrase, ifp)
            offset += size
            # perform size checks
            app_log.debug("performing minimal consistency check...")
            st = infile.stat()
            if hdr['data_sz'] >= st.st_size:
                app_log.warning("inconsistent size in deciphered header: corrupted header or bad passphrase.")
                return False
            # decrypt file data
            app_log.debug("decrypting data...")
            outfile = outdir.joinpath(self.filepath.name)
            self._decrypt_data(hdr['ckey'], hdr['salt'], ifp, outfile)
            return True
