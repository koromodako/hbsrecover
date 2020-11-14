'''HBSEncryptedFileV1 definition
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from ..meta import BaseHBSEncryptedFile

# ------------------------------------------------------------------------------
# CLASSES
# ------------------------------------------------------------------------------
class HBSEncryptedFileV1(BaseHBSEncryptedFile):
    MAGIC = b'__QCS__'
    VERSION = 1

    def decrypt(self, passphrase, outdir):
        """Write decrypted version of the file to outdir"""
        raise NotImplementedError
