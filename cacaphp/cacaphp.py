#!/usr/bin/env python
#
# cacaphp is a fucking php function fuzzer written in order to prove
# that php is badcoded.
#
import sys, os, random, pickle
from mangle_php import php_funcs, php_src

#
# globals
#
phpfilename = "foo/caca.php"    # fuzzed filename
phpfuncs    = "phpfuncs.pkl"    # php functions to fuzz
funcs       = []

#
# const
#
MAX_FUNCS = 100
MAX_ARGS = 10

#
# init funcs
#
def _cacainit():
    pkl_file = open(phpfuncs, 'rb')
    funcs = pickle.load(pkl_file)
    pkl_file.close()

def _makefuncs(func):
    return func + "(\"" +  + "\");\n"

def _makefakesrc(f):
    f.write("<?php\n")
    nbfuncs = random.randint(0, MAX_FUNCS)
    f.write(php_src()[1])
    for i in range(nbfuncs):
        f = funcs.values()[random.randint(0, len(funcs)-1)]
        if f.endswith("+"):
            na = random.randint(funcs[f], MAX_ARGS)
            ff = f[:-1]
        else:
            na = funcs[f]
            ff = f
        f.write(php_funcs(ff, na))
    f.write("?>\n")

#
# main, no arg is required.
#
def main(argv):
    # seedage
    if len(argv) == 2:
        random.seed(int(argv[1]))
    else:
        random.seed(1337)
    # init
    _cacainit()
    # open
    try:
        f = open(phpfilename, "w")
    except:
        print "open fucked"
        sys.exit(1)

    _makefakesrc(f)
    f.close()

if __name__ == '__main__':
    main(sys.argv)
