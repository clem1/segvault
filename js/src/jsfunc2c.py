#!/usr/bin/env python
#
# spidermonkey function parser
#

import re
import sys
import glob

#
# static JSFunctionSpec math_static_methods[] = {
#    {"abs",             math_abs,               1, 0, 0},
#    {"acos",            math_acos,              1, 0, 0},
#    {"asin",            math_asin,              1, 0, 0},
#    {"atan",            math_atan,              1, 0, 0},
#    {"atan2",           math_atan2,             
#
jsfuncspec_rx = re.compile(""".*?static JSFunctionSpec (\w+?)(_static){0,1}_methods\[\]\s*=\s*{(.*?)};(.*)""", re.DOTALL)
jsfunc_rx = re.compile("""\s*{\s*"(\w+)""")
jsfunc_rx2 = re.compile("""\s*{\s*js_(\w+)_str,""")

jsstaticclasses = []
jsfuncs = {}
for f in glob.glob("*c"):
    #print "parsing", f, "..."
    src = open(f, 'r').read()
    while src:
        try:
            m = jsfuncspec_rx.match(src)
            classname, static, funcs, src = m.groups()
            print "found class", classname
            if not jsfuncs.has_key(classname):
                jsfuncs[classname] = []
            if static:
                jsstaticclasses.append(classname)
            for l in funcs.split("\n"):
                mm = jsfunc_rx.match(l)
                if not mm:
                    mm = jsfunc_rx2.match(l)
                if mm:
                    funcname = mm.groups()[0]
                    #print "found function", funcname
                    #if static:
                    #    print '"%c%s.%s",' % (classname[0].upper(), classname[1:], funcname),
                    #else:
                    #    print '"%s.%s",' % (classname[0:2], funcname),
        except:
            src = None
