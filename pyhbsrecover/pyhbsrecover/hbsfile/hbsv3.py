'''HBSEncryptedFileV3 definition
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from pprint import pformat
from ..meta import BaseHBSEncryptedFile
from ..crypto import (
    AES_IV_SZ,
    AES_KEY_SZ,
    openssl_pbkdf,
    aes_cbc_decrypt_data,
)
from ..logging import app_log
# ------------------------------------------------------------------------------
# CLASSES
# ------------------------------------------------------------------------------
class HBSEncryptedFileV3(BaseHBSEncryptedFile):
    MAGIC = b'Salted__'
    VERSION = 3

    HDR_SPECS = {
        'magic': '8c',
        'salt': '8c',
    }

    def _parse_header(self, ifp):
        '''Parse file header
        '''
        hdr_sz = self._header_size(self.HDR_SPECS)
        hdr = self._unpack_header(self.HDR_SPECS, ifp.read(hdr_sz))
        hdr['magic'] = b''.join(hdr['magic'])
        hdr['salt'] = b''.join(hdr['salt'])
        app_log.debug("header:\n%s", pformat(hdr))
        return hdr_sz, hdr

    def decrypt(self, passphrase, outdir):
        '''Write decrypted version of the file to outdir
        '''
        with self.filepath.open('rb') as ifp:
            offset = 0
            size, hdr = self._parse_header(ifp)
            offset += size
            key, iv = openssl_pbkdf(AES_KEY_SZ, AES_IV_SZ,
                                    hdr['salt'], passphrase, 1)
            st = self.filepath.stat()
            outfile = outdir.joinpath(self.filepath.name)
            aes_cbc_decrypt_data(key, iv, ifp, st.st_size - offset, outfile)
            return True
