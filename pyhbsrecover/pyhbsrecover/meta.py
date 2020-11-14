'''Meta classes and common interface for HBSEncryptedFile
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from struct import unpack, calcsize

# ------------------------------------------------------------------------------
# CLASSES
# ------------------------------------------------------------------------------
class MetaHBSEncryptedFile(type):
    """"""

    BASE_CLASSES = {'BaseHBSEncryptedFile'}
    MANDATORY_CONSTS = {'VERSION', 'MAGIC'}
    VERSION_MAGIC_MAP = {}
    VERSION_CLASS_MAP = {}

    def __new__(cls, name, bases, dct):
        """Process new classes having this metaclass"""
        ncls = super().__new__(cls, name, bases, dct)
        # skip base classes
        if name in MetaHBSEncryptedFile.BASE_CLASSES:
            return ncls
        # process subclasses
        for mandatory_const in MetaHBSEncryptedFile.MANDATORY_CONSTS:
            if not mandatory_const in dct:
                raise RuntimeError(
                    f"Subclasses must define '{mandatory_const}'"
                )
        version = dct['VERSION']
        if version in MetaHBSEncryptedFile.VERSION_MAGIC_MAP:
            raise RuntimeError(
                f"version ({version}) already defined in metaclass structures."
            )
        # fill meta structures
        MetaHBSEncryptedFile.VERSION_MAGIC_MAP[version] = dct['MAGIC']
        MetaHBSEncryptedFile.VERSION_CLASS_MAP[version] = ncls
        return ncls


# pylint: disable=E1101


class BaseHBSEncryptedFile(metaclass=MetaHBSEncryptedFile):
    """Common interface for all HBSEncryptedFile versions"""

    def __init__(self, filepath):
        """Constructor"""
        if not filepath.is_file():
            raise RuntimeError(
                "HBSEncryptedFile.filepath shall point to a regular file!"
            )
        self._filepath = filepath

    @property
    def magic(self):
        return self.MAGIC

    @property
    def version(self):
        return self.VERSION

    @property
    def filepath(self):
        """Path to encrypted file"""
        return self._filepath

    def _header_size(self, specs):
        """Compute header size according to specs"""
        return sum([calcsize(fmt) for fmt in specs.values()])

    def _unpack_header(self, specs, data):
        """File header according to specs"""
        header = {}
        offset = 0
        for key, fmt in specs.items():
            fmt_sz = calcsize(fmt)
            header[key] = unpack(fmt, data[offset : offset + fmt_sz])
            offset += fmt_sz
        return header

    def fingerprint(self):
        """Display a fingerprint of the file"""
        print(f"{self.filepath}: version={self.version}")

    def decrypt(self, passphrase, outdir):
        """Write decrypted version of the file to outdir"""
        raise NotImplementedError(
            "HBSEncryptedFile subclasses shall define .decrypt() method"
        )


# ------------------------------------------------------------------------------
# FUNCTIONS
# ------------------------------------------------------------------------------
def identify_hbs_file(filepath):
    """Identify an HBS encrypted file and return appropriate class or None"""
    found = None
    with filepath.open('rb') as ifp:
        for version, magic in MetaHBSEncryptedFile.VERSION_MAGIC_MAP.items():
            ifp.seek(0)
            if ifp.read(len(magic)) == magic:
                found = version
                break
    if found:
        return MetaHBSEncryptedFile.VERSION_CLASS_MAP[found]
    return None
