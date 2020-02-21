'''HBSEncryptedFileV2 definition
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from pprint import pformat
from ..meta import BaseHBSEncryptedFile
from ..crypto import (
    AES_KEY_SZ,
    get_aes_ecb_cipher,
    aes_cbc_decrypt_data,
)
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
        hdr_sz = self._header_size(self.HDR_SPECS)
        ciphered = ifp.read(hdr_sz)
        key = passphrase * (1 + AES_KEY_SZ // len(passphrase))
        cipher = get_aes_ecb_cipher(key[:AES_KEY_SZ])
        deciphered = cipher.decrypt(ciphered)
        hdr = self._unpack_header(self.HDR_SPECS, deciphered)
        # postprocess header
        hdr['magic'] = b''.join(hdr['magic'])
        hdr['ckey'] = b''.join(hdr['ckey'])
        hdr['salt'] = b''.join(hdr['salt'])
        hdr['data_sz'] = hdr['data_sz'][0]
        app_log.debug("header:\n%s", pformat(hdr))
        return hdr_sz, hdr

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
            st = self.filepath.stat()
            if hdr['data_sz'] >= st.st_size:
                app_log.warning("inconsistent size in deciphered header: corrupted header or bad passphrase.")
                return False
            # decrypt file data
            app_log.debug("decrypting data...")
            outfile = outdir.joinpath(self.filepath.name)
            aes_cbc_decrypt_data(hdr['ckey'], hdr['salt'],
                                 ifp, st.st_size - offset, outfile)
            return True
