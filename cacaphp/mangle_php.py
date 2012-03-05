#!/usr/bin/env python
#
# mangle_php.py : mangle PHP code !#@
#
#
from random import randint, choice as random_choice
import sys, pickle, mangle
from array import array

# global
MAX_ARRAY_SIZE = 5
MAX_VARNAME_SIZE = 5
MAX_VAR_INIT = 2
MAX_FUNCS = 10
MAX_ARGS  = 10
phpfuncs = "phpfuncs.pkl"       # php functions to fuzz
phpmethods = "phpmethods.pkl"   # php methods to fuzz
funcs = {}
methods = {}

VARS = "abcde"

def _varname():
    """return a dummy var name"""
    return random_choice(VARS)

def _var():
    """return random var"""
    return "$" + _varname()

def _varvar():
    """return random varvar"""
    return "$$a"

def _array():
    """return random name"""
    return "@" + _varname()

def _phpvalue(names = [ "$CACAPHP" ]):
    """return a php value or a varname"""
    what = randint(0, 15)
    if what == 0:
        # string
        return mangle.string()
    elif what == 1 or what == 2:
        # integer
        return mangle.integer()
    elif what == 3:
        # var
        return _var()
    elif what == 4:
        # varvar
        return _varvar()
    elif what == 5:
        # ref
        return "&$a"
    elif what == 6 or what == 7:
        # class
        return "new dummy()"
    elif what == 8:
        # big string
        return """str_repeat("A", 200000000)"""
    elif what == 9 or what == 10:
        # crypt() string
        return "\"$%d$%s\"" % (randint(1, 500), mangle.string()[1:-1])
    elif what == 11:
        return "emptyfile"
    elif what == 12:
        return mangle.regex()
    elif what == 13:
        return mangle.html()
    return mangle.string()

def _args(nb):
    """return a string to put into a func()"""
    args = ""
    for i in range(nb-1):
        args += str(_phpvalue()) + ", "
    args += str(_phpvalue())
    return args

def php_init():
    pkl_file = open(phpfuncs, 'rb')
    fs = pickle.load(pkl_file)
    # HAHAHA!
    for f in fs.keys():
        funcs[f] = fs[f]
    pkl_file.close()
    pklm_file = open(phpmethods, 'rb')
    ms = pickle.load(pklm_file)
    # HAHAHA !
    for m in ms.keys():
        methods[m] = {}
        for f in ms[m].keys():
            methods[m][f] = ms[m][f]
    pklm_file.close()

def php_array():
    """return a dummy php array"""
    asize = randint(0, MAX_ARRAY_SIZE)
    aname = _varname()
    parray = '$' + aname + " = array("
    for i in range(asize):
        parray += str(_string()) + " => " + str(_phpvalue()) + ",\n"
    parray += str(_string()) + " => " + str(_phpvalue()) + ");"
    return ('@' + aname, parray)

def php_funcs(func, param = 2):
    """return func with random param"""
    v = _varname()
    if param != 0:
        return "$" + v + " = " + func + "(" + _args(param) + ");\n"
    else:
        return "$" + v + " = " + func + "();\n"

def php_methods(m, func, param = 2):
    """return func with random param"""
    v = _varname()
    if param != 0:
        return "$" + v + " = " + m + "->" + func + "(" + _args(param) + ");\n"
    else:
        return "$" + v + " = " + m + "->" + func + "();\n"

def php_var():
    """return a dummy var"""
    vname = _var()
    return (vname, vname + " = " + str(_phpvalue()) + ";")

def php_postsrc():
    """return php src with all names of predefined var"""
    src = ""
    occ = randint(0, MAX_VAR_INIT)
    names = []
    for i in range(occ):
        what = randint(0, 1)
        if what == 0:
            # array
            a = php_array()
        elif what == 1:
            # var
            a = php_var()
        names.append(a[0])
        src += a[1] + "\n"
    return (names, src)

def php_src(f):
    f.write("<?php\n")
    f.write("class dummy\n")
    f.write("{\n")
    f.write("    function __toString()\n")
    f.write("    {\n")
    f.write("""        parse_str("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx=1", $GLOBALS['var']);\n""")
    f.write("""        return "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";\n""")
    f.write("    }\n")
    f.write("    function __sleep()\n")
    f.write("    {\n")
    f.write("            shm_detach($GLOBALS['r']);\n")
    f.write("""          $GLOBALS['xxxxxxxxxxxxxxxxxxxxxxxxxxxxA'] = "AAAAAAAABBB";\n""")
    f.write("""          $GLOBALS['xxxxxxxxxxxxxxxxxxxxxxxxxxxxx'] = "AAAAAAAABBBBBBBBCCCCCCC";\n""")
    f.write("            return array();\n")
    f.write("    }\n")
    f.write("}\n\n")
    f.write("""$a = 42;\n$b = Array('hello', 10, 3);\n$c = str_repeat("A", 16000000);\n$d = null;\n$e = 3.14;\n""")
    if randint(0, 5) == 2:
        m = methods.keys()[randint(0, len(methods)-1)]
        f.write("$x = new " + m + "();\n")
        #f.write("$x->open('foo.zip');\n")
        for i in range(randint(1, 8)):
            fu = methods[m].keys()[randint(0, len(methods[m])-1)]
            if fu.endswith("+"):
                na = randint(methods[m][fu], MAX_ARGS)
                ff = fu[:-1]
            else:
                na = methods[m][fu]
                ff = fu
            #f.write("if (function_exists('" + ff + "')) {\n\t")
            #f.write("echo ('" + ff + "');\n")
            f.write(php_methods("$x", ff, na)  + "\n")

    nbfuncs = randint(0, MAX_FUNCS)
    for i in range(nbfuncs):
        fu = funcs.keys()[randint(0, len(funcs)-1)]
        if fu.endswith("+"):
            na = randint(funcs[fu], MAX_ARGS)
            ff = fu[:-1]
        else:
            na = funcs[fu]
            ff = fu
        #f.write("if (function_exists('" + ff + "')) {\n\t")
        f.write("echo ('" + ff + "');\n\t")
        f.write(php_funcs(ff, na) + "\n")
        #f.write("}\n")
    f.write("?>\n")

def main():
    print php_var()[1]
