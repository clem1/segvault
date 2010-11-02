
from xml.parsers.xmlproc import dtdparser, xmlapp
import string
import random

protos = open("dtds/protos").readlines()

dict_meta_characters = [
    '~',
    '`',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '-',
    '_',
    '+',
    '=',
    '{',
    '}',
    '[',
    ']',
    '\\',
    '|',
    ';',
    ':',
    "'",
    '"',
    '<',
    '>',
    ',',
    '.',
    '/',
    '?'
]

dict_string_values = [
    '.',
    '..',
    '.' * 10000,
    '%00',
    '%00' * 10000,
    '\x00',
    '\x00' * 1000,
    '\x0A',
    '\x0A' * 1000,
    '\x0D',
    '\x0D' * 1000,
    '\x0D\x0A',
    '\x0D\x0A' * 1000,
    '&#x00;',
    '&#x00;' * 1000,
    '&#x0A;',
    '&#x0A;' * 1000,
    '&#x0D;',
    '&#x0D;' * 1000,
    '&#x0D;&#x0A;',
    '&#x0D;&#x0A;' * 1000,
    '../' * 1000,
    '..\\' * 1000,
    "'",
    "'" * 1000,
    '"',
    '"' * 1000,
    '>',
    '<',
    '<>',
    '<' + 'A' * 100 + '>',
    '&' + 'A' * 100 + ';'
    'A' * 10000,
    "'" + 'A' * 1000 + "'",
    'A' * 1000,
    "%x%x%x%x%x%n%n%n%n%n",
]

dict_number_values = [
    0,
    -1,
    0x100,
    0x1000,
    0x10000,
    0x100000,
    0xffffffff,
    0xfffffffe,
    0x80000000,
    0x7fffffff,
    0x7ffffffe,
    0x3fffffff
]

dict_special_strings = [
    '..',
    '../',
    '..\\',
    '%00',
    '%0A',
    '%0D',
    '%0D%0A',
    '&#x00;',
    '&#x0A;',
    '&#x0D;',
    '&#x0D;&#x0A;'
]

dict_plain_strings = [
    'A' * 100,
    'A' * 1000,
]

dict_mimes = [
    'application/asx',
    'application/divxplayer-plugi',
    'application/futuresplas',
    'application/itunes-plugi',
    'application/ogg',
    'application/pdf',
    'application/sdp',
    'application/vnd.adobe.x-mars',
    'application/vnd.adobe.xdp+xml',
    'application/vnd.adobe.xfd+xm',
    'application/vnd.adobe.xfdf',
    'application/vnd.fdf',
    'application/x-divxcontentuploa',
    'application/x-dr',
    'application/x-drm-v',
    'application/x-google-vlc-plugi',
    'application/x-mpeg',
    'application/x-mplayer2',
    'application/x-ogg',
    'application/x-rtsp',
    'application/x-sdp',
    'application/x-shockwave-flash',
    'application/x-silverligh',
    'application/x-vlc-plugin',
    'audio/3gpp',
    'audio/3gpp2',
    'audio/AMR',
    'audio/aac',
    'audio/aiff',
    'audio/basic',
    'audio/mid',
    'audio/midi',
    'audio/mp4',
    'audio/mpeg',
    'audio/vnd.qcel',
    'audio/wa',
    'audio/x-aac',
    'audio/x-aiff',
    'audio/x-caf',
    'audio/x-gsm',
    'audio/x-m4',
    'audio/x-m4a',
    'audio/x-m4p',
    'audio/x-midi',
    'audio/x-mpeg',
    'audio/x-ms-wax',
    'audio/x-ms-wma',
    'audio/x-wav',
    'image/pict',
    'image/png',
    'image/x-macpaint',
    'image/x-pict',
    'image/x-png',
    'image/x-quicktime',
    'image/x-sgi',
    'image/x-targ',
    'video/3gp',
    'video/3gpp2',
    'video/div',
    'video/flc',
    'video/mp4',
    'video/mpeg',
    'video/quicktime',
    'video/sd-video',
    'video/x-m4v',
    'video/x-mpe',
    'video/x-ms-asf',
    'video/x-ms-asf-plugin',
    'video/x-ms-wm',
    'video/x-ms-wmv',
    'video/x-ms-wv'
]

def fuzz_randurl():
    """
    return a random fuzzed URL
    """
    proto = random.choice(protos)
    url = ""
    for i in xrange(0, random.randint(0, 2)):
        url += fuzz_randstring() + "/"
    url += fuzz_randstring()
    return proto.strip() + "://" + url

def fuzz_xmlattr():
    """
    return a random fuzzed XML attribute
    """
    what = random.randint(0, 10)
    if what == 0:
        return random.choice(dict_string_values)
    elif what == 1:
        return random.choice(dict_number_values)
    elif what == 2:
        return random.choice(dict_special_strings)
    elif what == 3:
        return random.choice(dict_plain_strings)
    elif what == 4:
        return random.choice(dict_mimes)
    elif what == 5:
        return fuzz_randurl()
    elif what == 6:
        return random.choice(dict_meta_characters) * random.randint(0, 15)
    else:
        return fuzz_randstring()
    return "f000"

def fuzz_randstring():
    """
    return random string
    """
    foostr = [ "coin", "gni", "bar", "pouette" ]
    proto = random.choice(protos)
    what = random.randint(0, 8)
    if what == 0:
        return "A"*random.randint(1, 200)
    elif what == 1:
        return "\\\\\\\\\\\\\\\\\\\\%x"*random.randint(0,50)+"%n"*random.randint(0,10)
    elif what == 2:
        return str(chr(random.randint(ord('A'), ord('z')))) * random.randint(0,10)
    elif what == 3:
        s = ""
        for i in xrange(0, random.randint(0, 15)):
            s += "%%%x" % random.randint(0, 255)
        for i in xrange(0, random.randint(0, 15)):
            s += "%%%x" % random.randint(0, 65535)
        return s
    elif what == 4:
        return "../" * random.randint(0, 20)
    elif what == 5:
        return "#%x" % random.randint(0, 0xFFFFFF)
    elif what == 6:
        return "%s%s" % (random.choice(dict_meta_characters), random.choice(foostr))
    elif what == 7:
        return random.choice(foostr)

    return "coin"

class XMLGen:

    """
    Generate fuzzed SVG based on DTD
    """
    xml_types = {}
    xml_types["svg11"] = {"header": """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/TR/SVG/DTD/svg11.dtd">\n""", "root": """<svg xmlns="http://www.w3.org/2000/svg" version="1.1">\n""", "endroot": """</svg>\n""" }
    xml_types["svg10"] = {"header": """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/SVG/DTD/svg10.dtd">\n""", "root": """<svg xmlns="http://www.w3.org/2000/svg" version="1.0">\n""", "endroot": """</svg>\n""" }
    xml_types["xhtml1"] = {"header": """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">""", "root": """<html>""", "endroot": "</html>" }

    def __init__(self, dtd, xmltype="svg11"):
        self.elems = {}
        p = dtdparser.DTDParser()
        p.set_dtd_consumer(DTDConsume(self.elems))
        p.parse_resource(dtd)
        self.toend = []
        self.xmlt = xmltype

    def fuzz(self):
        nelem = random.randint(1, 80)
        data = self.xml_types[self.xmlt]["header"] + self.xml_types[self.xmlt]["root"]
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
        data += self.xml_types[self.xmlt]["endroot"]
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
