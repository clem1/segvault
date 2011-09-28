#!/usr/bin/python
"""
Bits shifting Fuzzer using hachoir parsers

TODO: crc32 and co fixup!
"""
from hachoir_parser import createParser
from hachoir_core.i18n import getTerminalCharset
from sys import stderr, exit, argv
from bitarray import bitarray
from random import randint
from app_unix import Application
from optparse import OptionParser
from os.path import basename

class FuzzParser:
    def __init__(self, filename, quiet=True):
        self.savedbits = bitarray()
        self.savedbits.fromfile(open(filename, 'rb'))
        self.bgs = []
        self.bgn = []
        self.bgi = []
        self.quiet = quiet
        self.filename = filename
        self._parse(filename)
        print "%s parser (found %u fuzzable items)" % (filename, len(self.bgs))

    def _parse(self, filename):
        self._fields_parse(0, createParser(unicode(filename, getTerminalCharset())))

    def _fields_parse(self, idx, parser):
        for field in parser:
            self._field_parse(idx, field)

    def _field_parse(self, idx, field):
        if field.is_field_set:
            self._fields_parse(idx, field)
        elif field.size:
            self.bgs.append(field.size)
            self.bgn.append(field.name)
            self.bgi.append(idx)
            idx += field.size

    def fuzz(self, num):
        bits = self.savedbits.copy()
        for i in range(randint(1, 4)):
            rf = -1
            while rf == -1 or self.bgs[rf] > 128:
                rf = randint(0, len(self.bgs)-1)
            if not self.quiet:
                print "[%d] Fuzzing %s (%d)" % (num, self.bgn[rf], self.bgs[rf])
            self._fuzzbits(bits, self.bgi[rf], self.bgs[rf])

        return bits

    def _fuzzbits(self, bits, idx, size):

        choice = randint(0, 9)
        if choice < 5:
            # random bit shifting
            for l in range(size):
                if randint(0, 2) == 1:
                    bits.shift(idx + l)
        elif choice == 5:
            # full bit shifting
            for l in range(size):
                bits.shift(idx + l)
        elif choice == 6:
            # all at 1
            for l in range(size):
                if not bits[idx+l]:
                    bits.shift(idx + l)
        elif choice == 7:
            # all at 0
            for l in range(size):
                if bits[idx+l]:
                    bits.shift(idx + l)
        elif choice == 8:
            # bit shift the first
            bits.shift(idx)
        elif choice == 9:
            # bit shift the last
            bits.shift(idx + size - 1)


def is_vuln(line):
    return line.find("Invalid read") >= 0 or line.find("Invalid write") >= 0

def run(prog, args, valgrind):
    if valgrind:
        app = Application("valgrind", ["--leak-check=no", "--quiet", prog] + args)
        app.pipeStderr()
        app.start()
        try:
            while app.wait(False):
                line = app.readline(timeout=None, stream="stderr")
                if line:
                    if is_vuln(line):
                        print line
                        app.stop()
                        return True
        except KeyboardInterrupt:
            print "Interrupt!"
            app.stop()
            return True
    else:
        app = Application(prog, args)
        app.start()
        try:
            app._wait(True)
        except KeyboardInterrupt:
            print "Interrupt!"
            app.stop()
            return True
    return app.exit_failure and app.exit_code is None

def parse_opts():
    parser = OptionParser("usage: %prog [options] file")
    parser.add_option("-v", "--valgrind", action="store_true", dest="use_valgrind", default=False, help="Use valgrind memcheck to find invalid r/w")
    parser.add_option("-o", "--occ", type="int", dest="occ", default=50, help="Number of generated fuzzed files before calling prog", metavar="OCC")
    parser.add_option("-p", "--prog", dest="prog", default="clamscan", help="Program to fuzz", metavar="PROG")
    parser.add_option("-d", "--dstpath", dest="dstpath", default="/tmp/fuzz", help="Path where fuzzed files are created", metavar="DSTPATH")
    parser.add_option("-q", "--quiet", dest="quiet", default="False", action="store_true", help="No fucking print")
    parser.add_option("-a", "--args", dest="args", default="", help="Program args")
    return parser.parse_args()


def main():
    # parse option
    options, args = parse_opts()
    if len(args) == 0:
        print "missing file to fuzz"
        exit(1)

    # create fuzzers
    fuzzparser = []
    for arg in args:
        fuzzparser.append(FuzzParser(arg, options.quiet))

    iter = 1
    while True:
        if not options.quiet:
            print "Fuzzing... (%u iters)" % iter

        # created fuzzed files
        for fp in fuzzparser:
            for i in range(options.occ):
                fname, fext = basename(fp.filename).split(".")
                fp.fuzz(i).tofile(open("/%s/%s-fuzzy-%02d.%s" % (options.dstpath, fname, i, fext), "wb"))

        # dir or single file?
        args = options.args.split(",")
        if options.occ == 1:
            args.append("/%s/%s-fuzzy-%02d.%s" % (options.dstpath, fname, 0, fext))
        else:
            args.append(options.dstpath)

        # launch our proggy against fuzzed files
        if run(options.prog, args, options.use_valgrind):
            print "Youhou, crashed!"
            exit(0)

        iter += 1

if __name__ == "__main__":
    main()
