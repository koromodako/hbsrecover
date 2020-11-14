'''
Inspired from https://github.com/Mikiya83/hbs_decipher
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
import sys
from pathlib import Path
from getpass import getpass
from argparse import ArgumentParser
from . import __banner__
from .meta import identify_hbs_file
from .logging import app_log, set_debug
from .hbsfile import *

# ------------------------------------------------------------------------------
# FUNCTIONS
# ------------------------------------------------------------------------------
def parse_args():
    """Parse command line arguments"""
    parser = ArgumentParser(
        description="QNAP Hybrid Backup Sync Recovery Tool"
    )
    parser.add_argument(
        '--debug', '-d', action='store_true', help="Enable debugging output."
    )
    parser.add_argument(
        '--outdir',
        type=Path,
        default=Path('pyhbsrecovery.out'),
        help="Output directory where decrypted files will be written",
    )
    parser.add_argument(
        '--fingerprint',
        action='store_true',
        help="Perform a fingerprint of input files only.",
    )
    parser.add_argument(
        'inpath', type=Path, help="Input can be a file or a directory"
    )
    args = parser.parse_args()
    set_debug(args.debug)
    app_log.debug(args)
    passphrase = 'fingerprint'
    if not args.fingerprint:
        while True:
            try:
                passphrase = getpass(
                    "Enter encryption passphrase (cannot be empty): "
                )
            except KeyboardInterrupt:
                print()
                app_log.warning("user canceled the operation!")
                sys.exit(1)
            if not passphrase:
                app_log.error("Passphrase cannot be empty!")
                continue
            break
    args.passphrase = passphrase.encode()
    return args


def decrypt_file(passphrase, inpath, outdir, fingerprint=False):
    """Perform file decryption or fingerprint"""
    hbfs_cls = identify_hbs_file(inpath)
    if not hbfs_cls:
        return
    hbsf = hbfs_cls(inpath)
    if fingerprint:
        hbsf.fingerprint()
        return
    hbsf.decrypt(passphrase, outdir)


def decrypt_dir(passphrase, inpath, outdir, fingerprint=False):
    """Decrypt files recursively"""
    newoutdir = outdir.joinpath(inpath.name)
    for filepath in inpath.iterdir():
        decrypt(passphrase, filepath, newoutdir, fingerprint)


def decrypt(passphrase, inpath, outdir, fingerprint=False):
    """Select between directory or file decryption and ensure outdir exists"""
    if not fingerprint:
        outdir.mkdir(parents=True, exist_ok=True)
    if inpath.is_dir():
        decrypt_dir(passphrase, inpath, outdir, fingerprint)
    else:
        decrypt_file(passphrase, inpath, outdir, fingerprint)


def app():
    """Entry point"""
    app_log.info("-" * 60)
    app_log.info(__banner__)
    app_log.info("-" * 60)
    args = parse_args()
    try:
        decrypt(args.passphrase, args.inpath, args.outdir, args.fingerprint)
    except:
        app_log.exception("Exception caught! Details below.")


if __name__ == '__main__':
    app()
