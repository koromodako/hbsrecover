'''HBSEncryptedFileV3 definition
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from ..meta import BaseHBSEncryptedFile
# ------------------------------------------------------------------------------
# CLASSES
# ------------------------------------------------------------------------------
class HBSEncryptedFileV3(BaseHBSEncryptedFile):
    MAGIC = b'Salted__'
    VERSION = 3

    def decrypt(self, passphrase, outdir):
        '''Write decrypted version of the file to outdir
        '''
        raise NotImplementedError
