#!/usr/bin/env python
from __future__ import with_statement
from os import getcwd, mkdir, chdir, unlink
from os.path import dirname, normpath, join as path_join, exists, isabs, expanduser
from subprocess import Popen, STDOUT
from optparse import OptionParser
from glob import glob

SVN = "svn"
PATCH = "patch"
SCRIPT_2TO3 = "2to3"
PY3K = "python3.0"

def parseOptions():
    """
    Create all command line options including Fusil options.
    """
    parser = OptionParser(usage="%s [options] destdir")
    parser.add_option("--py3k",
        help="Python 3.0 program path (default: %s)" % PY3K,
        type="str", default=PY3K)
    parser.add_option("--2to3", dest="conv2to3",
        help="2to3 program path (default: %s)" % SCRIPT_2TO3,
        type="str", default=SCRIPT_2TO3)
    options, arguments = parser.parse_args()

    if len(arguments) != 1:
        parser.print_help()
        exit(1)
    return options, arguments[0]

def getModuleDir(module):
    path = dirname(module.__file__)
    path = path_join(path, "..")
    return normpath(path)

class Convert:
    def __init__(self):
        options, self.destdir = parseOptions()
        if not isabs(self.destdir):
            self.destdir = path_join(getcwd(), self.destdir)
            self.destdir = normpath(self.destdir)
        self.py3k = expanduser(options.py3k)
        self.conv2to3 = expanduser(options.conv2to3)

    def chdir(self, path):
        print "[+] Change directory: %s" % path
        chdir(path)

    def runCommand(self, *command, **options):
        info = "[+] Run command: %s" % ' '.join(command)
        print info
        filename = path_join(self.destdir, "command.log")
        with open(filename, "wb") as fp:
            print >>fp, info
            print >>fp
            options['stdout'] = fp
            options['stderr'] = STDOUT
            process = Popen(command, **options)
            exitcode = process.wait()
        if exitcode != 0:
            print "Command error: exitcode=%s" % exitcode
            print "Read %s for the log" % filename
            exit(1)
        unlink(filename)

    def run2to3(self, *arguments):
        self.runCommand(self.py3k, self.conv2to3, "-w", "-n", *arguments)

    def run(self):
        if exists(self.destdir):
            print "Directory %s already exists! exit" % self.destdir
            exit(1)
        print "[+] Start conversion in %s" % self.destdir
        mkdir(self.destdir)

        # Get directories
        import fusil
        orig_fusil_dir = getModuleDir(fusil)
        import ptrace
        orig_ptrace_dir = getModuleDir(ptrace)
        fusil_dir = path_join(self.destdir, "fusil")
        ptrace_dir = path_join(self.destdir, "ptrace")

        # svn export
        self.chdir(orig_fusil_dir)
        self.runCommand(SVN, "export", ".", fusil_dir)
        self.chdir(orig_ptrace_dir)
        self.runCommand(SVN, "export", ".", ptrace_dir)

        # ptrace: 2to3 (code+doc)
        self.run2to3(ptrace_dir)
        self.run2to3("-d", ptrace_dir)

        # ptrace: patch
        self.chdir(ptrace_dir)
        with open(path_join(ptrace_dir, "python3.0.patch")) as fp:
            self.runCommand(PATCH, "-p1", stdin=fp)

        # fusil: 2to3
        args = [fusil_dir]
        args += glob(path_join(fusil_dir, "fuzzers/fusil-*"))
        args += glob(path_join(fusil_dir, "fuzzers/notworking/fusil-*"))
        self.run2to3(*args)

        # fusil: 2to3 (doc)
        args = ["-d"]
        args += glob(path_join(fusil_dir, "doc/*.rst"))
        args += glob(path_join(fusil_dir, "tests/*.rst"))
        self.run2to3(*args)

        # fusil: patch
        self.chdir(fusil_dir)
        with open(path_join(fusil_dir, "python3.0.patch")) as fp:
            self.runCommand(PATCH, "-p1", stdin=fp)

if __name__ == "__main__":
    Convert().run()

