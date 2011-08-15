#!/usr/bin/python
"""
Extract audio from a FLV movie
"""
from hachoir_parser import createParser
from hachoir_core.i18n import getTerminalCharset
from sys import stderr, exit, argv
from bitarray import bitarray
from random import randint
from app_unix import Application

class FuzzParser:
    def __init__(self, filename):
        self.savedbits = bitarray()
        self.savedbits.fromfile(open(filename, 'rb'))
        self.bgs = []
        self.bgn = []
        self.bgi = []
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
        else:
            self.bgs.append(field.size)
            self.bgn.append(field.name)
            self.bgi.append(idx)
            idx += field.size

    def fuzz(self):
        bits = self.savedbits.copy()
        for i in xrange(randint(1, 6)):
            rf = -1
            while rf == -1 or self.bgs[rf] > 64:
                rf = randint(0, len(self.bgs)-1)
            #print "fuzzing...%s" % self.bgn[rf]
            idx = self.bgi[rf]
            for l in xrange(self.bgs[rf]):
                if randint(0, 3) == 2:
                    bits.shift(idx)
                idx += 1
        return bits

def run(prog, args):
    app = Application(prog, args)
    app.start()
    try:
        app._wait(True)
    except KeyboardInterrupt:
        print "Interrupt!"
        app.stop()
        return True
    return app.exit_failure and app.exit_code is None

def main():
    if len(argv) != 2:
        print >>stderr, "usage: %s file" % argv[0]
        exit(1)

    # create fuzzer
    fuzzparser = FuzzParser(argv[1])

    iter = 1
    while True:
        print "Fuzzing... (%u iters)" % iter

        # created fuzzed files
        for i in xrange(500):
            fuzzparser.fuzz().tofile(open("/tmp/fuzz/fuzzy-%02d.%s" % (i, argv[1].split(".")[-1]), "wb"))

        # launch our proggy against fuzzed files
        if run("clamscan", ("/tmp/fuzz/",)):
            print "Youhou, crashed!"
            exit(0)

        iter += 1

if __name__ == "__main__":
    main()
