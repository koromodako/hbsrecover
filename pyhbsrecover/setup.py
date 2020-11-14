'''Setup file
'''
from pathlib import Path
from setuptools import setup, find_packages
from pyhbsrecover import __version__

HERE = Path(__file__).absolute().parent


def requirements():
    with HERE.joinpath('requirements.txt').open() as reqs:
        return list([req.strip() for req in reqs if req.strip()])


setup(
    # main information
    name='pyhbsrecover',
    version=__version__,
    description='Recover QNAP Hybrid Backup Sync encrypted files',
    author='koromodako',
    author_email='koromodako@gmail.com',
    url='https://github.com/koromodako/hbsrecover',
    # package files
    packages=find_packages(str(HERE)),
    install_requires=requirements(),
    # configuration files
    entry_points={
        'console_scripts': [
            'pyhbsrecover = pyhbsrecover.main:app',
        ]
    },
)
