#!/usr/bin/env python
from imp import load_source
from os import path
from sys import argv
from glob import glob

CLASSIFIERS = [
    'Intended Audience :: Developers',
    'Development Status :: 5 - Production/Stable',
    'Environment :: Console',
    'License :: OSI Approved :: GNU General Public License (GPL)',
    'Operating System :: OS Independent',
    'Natural Language :: English',
    'Programming Language :: Python',
]

MODULES = (
    "fusil",
    "fusil.linux",
    "fusil.mas",
    "fusil.network",
    "fusil.process",
)

SCRIPTS = glob("fuzzers/fusil-*")

def main():
    if "--setuptools" in argv:
        argv.remove("--setuptools")
        from setuptools import setup
        use_setuptools = True
    else:
        from distutils.core import setup
        use_setuptools = False

    fusil = load_source("version", path.join("fusil", "version.py"))
    PACKAGES = {}
    for name in MODULES:
        PACKAGES[name] = name.replace(".", "/")

    long_description = open('README').read() + open('ChangeLog').read()

    install_options = {
        "name": fusil.PACKAGE,
        "version": fusil.VERSION,
        "url": fusil.WEBSITE,
        "download_url": fusil.WEBSITE,
        "author": "Victor Stinner",
        "description": "Fuzzing framework",
        "long_description": long_description,
        "classifiers": CLASSIFIERS,
        "license": fusil.LICENSE,
        "packages": PACKAGES.keys(),
        "package_dir": PACKAGES,
        "scripts": SCRIPTS,
    }
    if use_setuptools:
        install_options["install_requires"] = ["python-ptrace>=0.6"]
    setup(**install_options)

if __name__ == "__main__":
    main()

