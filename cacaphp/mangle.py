#!/usr/bin/env python
#
# mangle.py : mangling code.
#
#
from random import randint, choice as random_choice

MAX_STRING_SIZE = 0xFFFFF
MAX_OPERATION = 4

# some proto://
PROTOS = ['bcmath', 'ctype', 'dom', 'ext_skel', 'ftp', 'hash', 'intl', 'mbstring', 'mysqli', 'openssl', 'pdo_dblib', 'pdo_odbc', 'phar', 'recode', 'simplexml', 'sockets', 'standard', 'sysvshm', 'xml', 'xsl', 'bz2', 'curl', 'enchant', 'ext_skel_win32.php', 'gd', 'iconv', 'json', 'mcrypt', 'mysqlnd', 'pcntl', 'pdo_firebird', 'pdo_pgsql', 'posix', 'reflection', 'skeleton', 'spl', 'sybase_ct', 'tidy', 'xmlreader', 'zip', 'calendar', 'date', 'ereg', 'fileinfo', 'gettext', 'imap', 'ldap', 'mssql', 'oci8', 'pcre', 'pdo_mysql', 'pdo_sqlite', 'pspell', 'session', 'snmp', 'sqlite', 'sysvmsg', 'tokenizer', 'xmlrpc', 'zlib', 'com_dotnet', 'dba', 'exif', 'filter', 'gmp', 'interbase', 'libxml', 'mysql', 'odbc', 'pdo', 'pdo_oci', 'pgsql', 'readline', 'shmop', 'soap', 'sqlite3', 'sysvsem', 'wddx', 'xmlwriter', 'file']

# printf formats
FORMATS = "dpsnf"

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

REGEX_CHARS="\\^.$|*+?!:-"

def regex():
    occ = randint(5, 20)
    r = "abc"
    for o in xrange(occ):
        w = randint(0, 10)
        if w >= 0 and w <= 3:
            r += random_choice(REGEX_CHARS)
        elif w == 4:
            r += "{%d, %d}" % (randint(0, 1024), randint(0, 1024))
        elif w == 5:
            r += "{%d,}" % randint(0, 1024)
        elif w == 6:
            l = randint(0, len(r)-1)
            r1, r2 = r[:l], r[l:]
            r = "(%s|%s)" % (r1, r2)
        elif w == 7:
            l = randint(0, len(r)-1)
            r1, r2 = r[:l], r[l:]
            r = "(%s)%s" % (r1, r2)
        elif w == 8:
            l = randint(0, len(r)-1)
            r1, r2 = r[:l], r[l:]
            r = "[%s]%s" % (r1, r2)
        elif w == 9:
            l = randint(0, len(r)-1)
            r1, r2 = r[:l], r[l:]
            r = "[^%s]%s" % (r1, r2)
    return r

def string():
    """return a random string"""
    string = '"'
    size = randint(15, MAX_STRING_SIZE)
    if randint(0, 5) > 3:
        string += random_choice(PROTOS) + "://"
    if randint(0, 5) > 3:
        string += "/"
    #for i in range(size):
    #   string += chr(randint(48, 122))
    string += "A" * (size - 15)
    string += '%0.262159f%s%jn%qu%s%S%s%s%n../../%s%n%555555n%x%x%x%x%x%n%n%n%n%x%x'
    if randint(0, 5) == 3:
        string += "%" + str(integer()) + "." + str(integer()) + random_choice(FORMATS)
    if randint(0, 5) == 3:
        string += "%" + str(integer()) + random_choice(FORMATS)
    if randint(0, 5) == 3:
        string += "%" + str(integer()) + "." + "*" * randint(0, 7) + random_choice(FORMATS)
    if randint(0, 5) == 3:
        string += "%." + str(integer()) + random_choice(FORMATS)
    string += '"'
    return string

def integer():
    """return a random integer"""
    what = randint(0, 2)
    if what == 0:
        return randint(0, 0xFFFFFFFF)
    elif what == 1:
        return -randint(0, 0xFFFFFFFF)
    elif what == 2:
        return random_choice(SPECIAL_VALUES)

def operation():
    """return random operation"""
    opps = [ '+', '-', '*', '/', '%' ]
    nbop = randint(0, MAX_OPERATION)
    op = ""
    for i in range(nbop):
        op += str(integer()) + random_choice(opps)
    op += str(integer())
    return op
