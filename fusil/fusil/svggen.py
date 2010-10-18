
from xml.parsers.xmlproc import dtdparser, xmlapp
import string
import random

def fuzz_xmlattr():
    """
    return a random fuzzed XML attribute
    """
    what = random.randint(0, 40)
    if what == 0:
        return 0xFFFF
    elif what == 1:
        return -1
    elif what == 2:
        return 0xFFFFFFFF
    elif what == 3:
        return 0x7FFFFFFF
    elif what == 4:
        return -0x7FFFFFFF
    elif what == 5:
        return 255
    elif what == 6:
        return 0
    elif what == 7:
        return 0x80000000
    elif what == 8:
        return "%x"*random.randint(0,500)+"%n"*random.randint(0,100)
    elif what == 9:
        return "A"*random.randint(1,50000)
    elif what == 10:
        return ((str(random.randint(-5000,50000)) + ",")*random.randint(1,500))[:-1]
    elif what == 11:
        return str(random.randint(-500, 50000)) + "%"
    elif what == 12:
        return str(random.randint(-500, 50000)) + "px"
    elif what == 13:
        return ((str(random.randint(-500,50000)) + "." + (str(random.randint(-500,50000))) + ",")*random.randint(1,500))[:-1]
    elif what == 14:
        return str(random.randint(0, 50000)) + "." + (str(random.randint(0,50000))) + "%"
    elif what == 15:
        return str(random.randint(0, 50000)) +"." + (str(random.randint(0,50000))) + "px"
    elif what > 15 and what < 30:
        return random.randint(0, 100000)
    else:
        return "ABCD"
    return "f000"

def fuzz_randstring():
    """
    return random string
    """
    what = random.randint(0, 2)
    if what == 0:
        return "A"*random.randint(1, 0x7FF)
    elif what == 1:
        return "%x"*random.randint(0,500)+"%n"*random.randint(0,100)
    elif what == 2:
        return str(chr(random.randint(ord('A'), ord('z')))) * random.randint(0,100)

class SVGGen:

    """
    Generate fuzzed SVG based on DTD
    """
    header  = """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/TR/SVG/DTD/svg11.dtd">\n"""
    root    = """<svg xmlns="http://www.w3.org/2000/svg" version="1.1">\n"""
    endroot = """</svg>\n"""

    def __init__(self, dtd):
        self.elems = {}
        p = dtdparser.DTDParser()
        p.set_dtd_consumer(DTDConsume(self.elems))
        p.parse_resource(dtd)
        self.toend = []

    def fuzz(self):
        nelem = random.randint(1, 80)
        data = self.header + self.root
        for i in xrange(0, nelem):
            # pick random elem
            elem = random.choice(self.elems.keys())
            if elem in ("text"):
                continue
            data += "<" + elem + " "
            # add attributes
            nattr = random.randint(1, 7)
            attr_done = []
            for ii in xrange(0, nattr):
                if len(attr_done) == len(self.elems[elem]):
                    break
                a = random.choice(self.elems[elem])
                while a in attr_done:
                    a = random.choice(self.elems[elem])
                attr_done.append(a)
                data += """ %s="%s" """ % (a, fuzz_xmlattr())
            # end
            end = (random.randint(0, 3) == 0)
            if end:
                data += "/>\n"
            else:
                data += ">\n"
                self.toend.append(elem)
                # add some data
                if random.randint(0, 10) == 0:
                    data += fuzz_randstring()
            # add some end attr
            if len(self.toend) > 0 and random.randint(0, 3) == 0:
                n = random.randint(0, len(self.toend))
                for ii in xrange(0, n):
                    data += "</%s>\n" % self.toend.pop()
        # end every not yet ended elem
        for i in xrange(0, len(self.toend)):
            data += "</%s>\n" % self.toend.pop()
        data += self.endroot
        return data

class DTDConsume(xmlapp.DTDConsumer):

    def __init__(self, elems):
        self._attlist = []
        self.elems = elems

    def new_general_entity(self, name, val):
        self._empty_attlist()

    def new_external_entity(self, name, pubid, sysid, ndata):
        self._empty_attlist()

    def new_parameter_entity(self, name, val):
        self._empty_attlist()

    def new_external_pe(self, name, pubid, sysid):
        self._empty_attlist()

    def new_notation(self, name, pubid, sysid):
        self._empty_attlist()

    def new_element_type(self, name, cont):
        self._empty_attlist()

    def new_attribute(self, elem, attr, a_type, a_decl, a_def):
        self._attlist.append((elem, attr, a_type, a_decl, a_def))

    def handle_comment(self, contents):
        self._empty_attlist()

    def handle_pi(self, target, data):
        self._empty_attlist()

    def _empty_attlist(self):
        prev = None
        for (elem, attr, a_type, a_decl, a_def) in self._attlist:
            if elem != prev:
                if not self.elems.has_key(elem):
                    self.elems[elem] = []
                prev = elem

            self.elems[elem].append(attr)
        if self._attlist != []:
            self._attlist = []

if __name__ == "__main__":
    svg = SVGGen('svg11-flat.dtd')
    print svg.fuzz()
