#!/usr/bin/env python
#
# mangle_php.py : mangle PHP code !#@
#
#
from random import randint, choice as random_choice
import sys, pickle
from array import array


# global
MAX_ARRAY_SIZE = 5
MAX_VARNAME_SIZE = 5
MAX_STRING_SIZE = 0xFFFF
MAX_OPERATION = 2
MAX_VAR_INIT = 2
MAX_FUNCS = 10
MAX_ARGS  = 10
phpfuncs = "phpfuncs.pkl"   # php functions to fuzz
funcs = {}

# from haypo's mangle.py
SPECIAL_VALUES = [
        0x7FFFFFFF,
        -1,
        0xFFFFFFFF,
        0x80000000,
        255,
        0,
        128,
        -0xFFFFFFFF,
        -128,
        0xFFFFFFFFF,
        0xFFFFFFFFFF,
        -50,
        5000000000000,
        0.50,
        ]

VARS = "abcde"

PROTOS = ['bcmath', 'ctype', 'dom', 'ext_skel', 'ftp', 'hash', 'intl', 'mbstring', 'mysqli', 'openssl', 'pdo_dblib', 'pdo_odbc', 'phar', 'recode', 'simplexml', 'sockets', 'standard', 'sysvshm', 'xml', 'xsl', 'bz2', 'curl', 'enchant', 'ext_skel_win32.php', 'gd', 'iconv', 'json', 'mcrypt', 'mysqlnd', 'pcntl', 'pdo_firebird', 'pdo_pgsql', 'posix', 'reflection', 'skeleton', 'spl', 'sybase_ct', 'tidy', 'xmlreader', 'zip', 'calendar', 'date', 'ereg', 'fileinfo', 'gettext', 'imap', 'ldap', 'mssql', 'oci8', 'pcre', 'pdo_mysql', 'pdo_sqlite', 'pspell', 'session', 'snmp', 'sqlite', 'sysvmsg', 'tokenizer', 'xmlrpc', 'zlib', 'com_dotnet', 'dba', 'exif', 'filter', 'gmp', 'interbase', 'libxml', 'mysql', 'odbc', 'pdo', 'pdo_oci', 'pgsql', 'readline', 'shmop', 'soap', 'sqlite3', 'sysvsem', 'wddx', 'xmlwriter']

def _varname():
    """return a dummy var name"""
    return random_choice(VARS)

def _string():
    """return a random string"""
    string = '"'
    if randint(0, 5) > 3:
        string += random_choice(PROTOS) + "://"
    if randint(0, 5) > 3:
        string += "/"
    size = randint(15, MAX_STRING_SIZE)
    #for i in range(size):
    #   string += chr(randint(48, 122))
    string += "A" * (size - 15)
    string += '%s%jn%qu%s%S%s%s%n%s%n%555555n%x%x%x%x%x%n%n%n%n%x%x'
    string += '"'
    return string

def _integer():
    """return a random integer"""
    what = randint(0, 1)
    if what == 0:
        return randint(0, 0xFFFFFFFF)
    else:
        return -randint(0, 0xFFFFFFFF)

def _evil():
    """return an evil thing"""
    return random_choice(SPECIAL_VALUES)

def _var():
    """return random var"""
    return "$" + _varname()

def _varvar():
    """return random varvar"""
    return "$$a"

def _array():
    """return random name"""
    return "@" + _varname()

def _opp():
    """return random opp"""
    opps = [ '+', '-', '*', '/', '%' ]
    nbop = randint(0, MAX_OPERATION)
    op = ""
    for i in range(nbop):
        op += str(_integer()) + random_choice(opps)
    op += str(_integer())
    return op

def _phpvalue(names = [ "$CACAPHP" ]):
    """return a php value or a varname"""
    what = randint(0, 10)
    if what == 0:
        # string
        return _string()
    elif what == 1:
        # integer
        return _integer()
    elif what == 2:
        # evil
        return _evil()
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
        return """str_repeat("A", 16000000)"""
    return _string()

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
    nbfuncs = randint(0, MAX_FUNCS)
    func = ""
#    f.write(php_postsrc()[1])
    for i in range(nbfuncs):
        fu = funcs.keys()[randint(0, len(funcs)-1)]
        if fu.endswith("+"):
            na = randint(funcs[fu], MAX_ARGS)
            ff = fu[:-1]
        else:
            na = funcs[fu]
            ff = fu
        if ff == "getservbyname" or ff == "getservbyport":
            continue
        #f.write("if (function_exists('" + ff + "')) {\n\t")
        f.write("echo ('" + ff + "');\n\t")
        f.write(php_funcs(ff, na))
        #f.write("}\n")
    f.write("?>\n")

def main():
    print php_var()[1]
