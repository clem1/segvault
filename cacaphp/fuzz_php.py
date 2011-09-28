#!/usr/bin/env python
from os import path
from app_unix import Application as BaseApplication
from array import array
from mangle_php import *
from time import sleep
from md5 import md5
from sys import argv, exit, stderr
from random import randint
from tools import ConfigParser, beNice, cleanupDir, safeMkdir
from timeout import limitedTime, Timeout

class Application(BaseApplication):
    def processExit(self, status):
        pass
        #if True:
        #    for line in self.readlines():
        #        print "output> %s" % line
        #else:
        #    line = None
        #    for line in self.readlines():
        #        pass
        #    if line:
        #        print "last line> %s" % line

class Fuzzer:
    def __init__(self, config, original):
        self.original = original
        self.data = open(self.original, 'rb').read()
        self.nb_file = config.getint("application", "nb_file")
        self.program = config.get('application', 'program')
        self.program_args = config.get('application', 'program_args').split()
        self.mangle_percent = config.getfloat('application', 'mangle_percent')
        self.timeout = config.getfloat('application', 'timeout')
        self.tmp_dir = config.get('application', 'tmp_dir')
        self.truncate_rate = config.getint("application", "truncate_rate")
        self.timeout_is_fatal = config.getboolean("application", "timeout_is_fatal")
        self.valgrind = config.getboolean("application", "valgrind")
        php_init()
        #assert 1 <= len(self.data)

    def is_vuln(self, line):
        return line.find("Invalid read") >= 0 or line.find("Invalid write") >= 0

    def fuzzFile(self, filenames):
        args = self.program_args + filenames
        if self.valgrind:
            app = Application("valgrind", ["--leak-check=no", "--quiet", self.program] + args)
            app.pipeStderr()
            app.start()
            try:
                while app.wait(False):
                    line = app.readline(timeout=None, stream="stderr")
                    if line:
                        if self.is_vuln(line):
                            print line
                            app.stop()
                            return True
            except KeyboardInterrupt:
                print "Interrupt!"
                app.stop()
                return True
        else:
            app = Application(self.program, args)
            app.start()
            try:
                limitedTime(self.timeout, app.wait)
            except Timeout:
                print "Timeout error!"
                app.kill()
                return self.timeout_is_fatal
            except KeyboardInterrupt:
                print "Interrupt!"
                app.stop()
                return True
        return app.exit_failure and app.exit_code is None

    def info(self, message):
        if False:
            print message

    def warning(self, message):
        print "WARN: %s" % message

    def mangle(self, filename):
        self.info("Mangle")
        output = open(filename, 'w')
        php_src(output)
        output.close()

    def run(self):
        ext = self.original[-4:]
        safeMkdir(self.tmp_dir)
        try:
            if 1 < self.nb_file:
                safeMkdir(self.tmp_dir)
            while True:
                if 1 < self.nb_file:
                    cleanupDir(self.tmp_dir)
                    files = []
                    for index in xrange(self.nb_file):
                        tmp_name = path.join(self.tmp_dir, 'image-%02u%s' % (index, ext))
                        self.mangle(tmp_name)
                        files.append(tmp_name)
                else:
                    tmp_name = path.join(self.tmp_dir, 'fuzz_app.file%s' % ext)
                    self.mangle(tmp_name)
                    files = [tmp_name]

                print "============= Run Test ==============="
                if self.fuzzFile(files):
                    print "Fuzzing error: stop!"
                    break
                print "Test: ok"
        except KeyboardInterrupt:
            print "Interrupt."

def main():
    global config

    # Read arguments
    if len(argv) != 3:
        print >>stderr, "usage: %s config.cfg filename" % argv[0]
        exit(1)
    config_file = argv[1]
    config = ConfigParser()
    config.read(["defaults.cfg", config_file])
    document = argv[2]

    # Be nice with CPU
    beNice()

    # Run fuzzer
    fuzzer = Fuzzer(config, document)
    fuzzer.run()

if __name__ == "__main__":
    main()

