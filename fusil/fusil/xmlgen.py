
from xml.parsers.xmlproc import dtdparser, xmlapp
import string
import random

#protos = open("dtds/protos").readlines()
#protos.extend(["http", "https"]*100)
protos = ["http", "https"]

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

dict_css_properties = [
    'azimuth',
    'background',
    'background-attachment',
    'background-color',
    'background-image',
    'background-position',
    'background-repeat',
    'border',
    'behaviour',
    'border-bottom',
    'border-bottom-color',
    'border-bottom-style',
    'border-bottom-width',
    'border-collapse',
    'border-color',
    'border-left',
    'border-left-color',
    'border-left-style',
    'border-left-width',
    'border-right',
    'border-right-color',
    'border-right-style',
    'border-right-width',
    'border-spacing',
    'border-style',
    'border-top',
    'border-top-color',
    'border-top-style',
    'border-top-width',
    'border-width',
    'bottom',
    'box-shadow',
    '-webkit-box-shadow',
    '-moz-box-shadow',
    'border-radius',
    '-moz-column-count',
    '-moz-column-width',
    'text-shadow',
    'box-sizing',
    'caption-side',
    'clear',
    'clip',
    'color',
    'content',
    'hanging-punctuation',
    'letter-spacing',
    'line-break',
    'punctuation-trim',
    'text-align',
    'text-align-last',
    'text-autospace',
    'text-decoration',
    'text-decoration-color',
    'text-decoration-line',
    'text-decoration-skip',
    'text-decoration-style',
    'text-emphasis',
    'text-emphasis-color',
    'text-emphasis-position',
    'text-emphasis-style',
    'text-indent',
    'text-justify',
    'text-outline',
    'text-overflow',
    'text-shadow',
    'text-transform',
    'text-underline-position',
    'text-wrap',
    'white-space',
    'white-space-collapsing',
    'word-break',
    'word-spacing',
    'word-wrap',
    'counter-increment',
    'counter-reset',
    'cue',
    'cue-after',
    'cue-before',
    'cursor',
    'direction',
    'display',
    'elevation',
    'empty-cells',
    'filter',
    'float',
    'font',
    'font-family',
    'font-size',
    'font-style',
    'font-variant',
    'font-weight',
    'height',
    'left',
    'letter-spacing',
    'line-height',
    'list-style',
    'list-style-image',
    'list-style-position',
    'list-style-type',
    'margin',
    'margin-bottom',
    'margin-left',
    'margin-right',
    'margin-top',
    'max-height',
    'max-width',
    'min-height',
    'min-width',
    '-moz-border-radius',
    '-moz-box-sizing',
    'orphans',
    'outline',
    'outline-color',
    'outline-style',
    'outline-width',
    'overflow',
    'padding',
    'padding-bottom',
    'padding-left',
    'padding-right',
    'padding-top',
    'page-break-after',
    'page-break-before',
    'page-break-inside',
    'pause',
    'pause-after',
    'pause-before',
    'pitch',
    'pitch-range',
    'play-during',
    'position',
    'quotes',
    'richness',
    'right',
    'speak',
    'speak-header',
    'speak-numeral',
    'speak-punctuation',
    'speech-rate',
    'stress',
    'src',
    'font-family',
    'font-weight',
    'font-style',
    'table-layout',
    'text-align',
    'text-decoration',
    'text-indent',
    'text-transform',
    'top',
    'unicode-bidi',
    'vertical-align',
    'visibility',
    'voice-family',
    'volume',
    'white-space',
    'widows',
    'width',
    'word-spacing',
    'z-index'
    'zoom',
    '-webkit-border-radius',
    '-moz-border-radius',
    '-moz-border-radius-topleft',
    '-webkit-border-top-left-radius',
    '-moz-border-radius-bottomright',
    '-webkit-border-bottom-right-radius',
    'border-image',
    'border-top-image',
    'border-right-image',
    'border-bottom-image',
    'border-left-image',
    'border-corner-image',
    'border-top-left-image',
    'border-top-right-image',
    'border-bottom-left-image',
    'border-bottom-right-image',
    '-moz-column-count',
    '-moz-column-gap',
    '-moz-column-rule',
    '-webkit-column-count',
    '-webkit-column-gap',
    '-webkit-column-rule',
    'resize',
    'outline-offset',
    'outline',
    'filter',
    'opacity',
]

dict_css_functions = [
    'url',
    'local',
    'format',
    'attr',
    'calc',
    'counter',
    'hsl',
    'cmyk',
    'hsla',
    'rgba',
    'alpha',
]

dict_css_func_selectors = [
    'nth-child',
    'nth-last-child',
    'nth-of-type',
    'nth-last-of-type',
    'not',

]

dict_css_selectors = [
    'root',
    'last-child',
    'first-of-type',
    'last-of-type',
    'only-child',
    'only-of-type',
    'empty',
    'target',
    'enabled',
    'disabled',
    'checked',
    'selection'
]

dict_css_values = [
    '100',
    '200',
    '300',
    '400',
    '500',
    '600',
    '700',
    '800',
    '900',
    'above',
    'absolute',
    'always',
    'armenian',
    'attr(0)',
    'rect(0)',
    'circle(0)',
    'auto',
    'avoid',
    'background-color',
    'baseline',
    'behind',
    'below',
    'bidi-override',
    'blink',
    'block',
    'bold',
    'bolder',
    'border-top-color',
    'both',
    'bottom',
    'capitalize',
    'caption',
    'center',
    'center-left',
    'center-right',
    'circle',
    'close-quote',
    'code',
    'collapse',
    'continuous',
    'crosshair',
    'decimal',
    'decimal-leading-zero',
    'default',
    'digits',
    'disc',
    'e-resize',
    'embed',
    'far-left',
    'far-right',
    'fast',
    'faster',
    'fixed',
    'georgian',
    'help',
    'hidden',
    'hide',
    'high',
    'higher',
    'icon',
    'inherit',
    'inline',
    'inline-block',
    'inline-table',
    'inside',
    'invert',
    'italic',
    'justify',
    'left',
    'left-side',
    'leftwards',
    'level',
    'lighter',
    'line-through',
    'list-item',
    'loud',
    'low',
    'lower',
    'lower-alpha',
    'lower-greek',
    'lower-latin',
    'lower-roman',
    'lowercase',
    'ltr',
    'medium',
    'menu',
    'message-box',
    'middle',
    'mix',
    'move',
    'n-resize',
    'ne-resize',
    'no-close-quote',
    'no-open-quote',
    'no-repeat',
    'none',
    'normal',
    'nowrap',
    'nw-resize',
    'oblique',
    'once',
    'open-quote',
    'outside',
    'overline',
    'pointer',
    'pre',
    'pre-line',
    'pre-wrap',
    'progress',
    'relative',
    'repeat',
    'repeat-x',
    'repeat-y',
    'right',
    'right-side',
    'rightwards',
    'rtl',
    'run-in',
    's-resize',
    'scroll',
    'se-resize',
    'separate',
    'show',
    'silent',
    'slow',
    'slower',
    'small-caps',
    'small-caption',
    'soft',
    'spell-out',
    'square',
    'static',
    'status-bar',
    'sub',
    'src',
    'super',
    'sw-resize',
    'table',
    'table-caption',
    'table-cell',
    'table-column',
    'table-column-group',
    'table-footer-group',
    'table-header-group',
    'table-row',
    'table-row-group',
    'text',
    'text-bottom',
    'text-top',
    'top',
    'transparent',
    'underline',
    'upper-alpha',
    'upper-latin',
    'upper-roman',
    'uppercase',
    'url',
    'visible',
    'w-resize',
    'wait',
    'x-fast',
    'x-high',
    'x-loud',
    'x-low',
    'x-slow',
    'x-soft',
    'break-word',
    'suppress',
    'newspaper',
    'keep-all'
]

def fuzz_randurl():
    """
    return a random fuzzed URL
    """
    proto = random.choice(protos)
    url = ""
    for i in xrange(0, random.randint(0, 3)):
        url += str(fuzz_randstring()) + "/"
    url += str(fuzz_randstring())
    return proto.strip() + "://" + url

def fuzz_xmlattr():
    """
    return a random fuzzed XML attribute
    """
    what = random.randint(0, 10)
    if what == 0:
        return random.choice(dict_string_values)
    elif what == 1:
        return str(random.choice(dict_number_values))
    elif what == 2:
        return random.choice(dict_special_strings)
    elif what == 3:
        return random.choice(dict_plain_strings)
    elif what == 4:
        return random.choice(dict_mimes)
    elif what == 5:
        return fuzz_randurl()
    elif what == 6:
        return str(fuzz_randstring()) + str(random.choice(dict_meta_characters)) + str(fuzz_randstring())
    else:
        return str(fuzz_randstring())
    return "f000"

def fuzz_randstring():
    """
    return random string
    """
    foostr = [ "javascript:", "style", "#", "|", "&", "#haha", ".", "^", "$", "A"*5000, "coin", "gni", "bar", "pouette", "\\", "\"", "^", "$", "*", "}", "/", "`" ]
    proto = random.choice(protos)
    what = random.randint(0, 15)
    if what == 0:
        return "A"*random.randint(1, 20000)
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
        a = [ "%", "px", "pt", "em" ]
        return "%d%s" % (fuzz_evilint(), random.choice(a))
    elif what == 8:
        s = ""
        for i in xrange(random.randint(1, 500)):
            s += str(random.randint(0, 255))
        return s
    elif what == 9:
        return str(fuzz_evilint())
    elif what == 10:
        s = "%s%s" % (str(fuzz_evilint()), random.choice(dict_meta_characters))
        s += "A%s%sA%s" % (random.choice(foostr), random.choice(dict_meta_characters), random.choice(foostr))
        s += "I%s%sI%s" % (random.choice(foostr), random.choice(dict_meta_characters), random.choice(foostr))
        return s
    else:
        return random.choice(foostr)*random.randint(1, 15)

    return "coin"

def fuzz_evilint():
    ei = ( 0xFF, 0xFFFFFFFF, 0xFFFF, -255, 0, -0xFFFF, -0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF+1, 254, 32, 64, 63, -80, 0xDEADBEEF, 0xC000 )
    if random.randint(0, 10) == 5:
        return random.choice(ei)
    return random.randint(0, 4096)

class CSSFuzz:
    """
    Generate random fuzzed CSS
    """
    def __init__(self, dtd):
        self.elems = {}
        self.elems["@font-face"] = ()
        self.elems[":root"] = ()
        p = dtdparser.DTDParser()
        p.set_dtd_consumer(DTDConsume(self.elems))
        p.parse_resource(dtd)
        self.toend = []

    def fuzz(self, elems=None):
        data = ""
        done = []
        if not elems:
            elems = []
            nelem = random.randint(1, 30)
            for i in xrange(0, nelem):
                elems.append(random.choice(self.elems.keys()))
        elems.append("@font-face")
        elems.append(":root")
        for elem in elems:
            if elem in done:
                continue
            done.append(elem)
            data += "%s%s {\n" % (elem, self.fuzz_cssselectors())
            for i in xrange(0, random.randint(1, 15)):
                p = random.choice(dict_css_properties)
                w = random.randint(1, 5)
                if w == 2:
                    a = self.fuzz_cssfunc()
                elif w == 4:
                    a = ""
                    for i in xrange(0, random.randint(1, 10)):
                        a += self.fuzz_cssfunc() + ","
                    a = a[:-1]
                elif w == 3:
                    a = fuzz_xmlattr()
                else:
                    a = self.fuzz_cssattr()
                data += "%s: %s;\n" % (p, a)
            data += "}\n"
        return data

    def fuzz_cssfunc(self):
        na = random.randint(1, 3)
        func = random.choice(dict_css_functions)
        args = ""
        for x in xrange(na):
            if random.randint(0, 3) == 2:
                args += "\"" + str(fuzz_xmlattr()) + "\"" + ","
            else:
                args += str(fuzz_xmlattr()) + ","

        return func + "(" + args[:-1] + ")"

    def fuzz_cssattr(self):
        s = ""
        m = random.randint(0, 30)
        if m == 1:
            n = random.randint(1, 50000)
            same = random.randint(0,1)
            if same:
                a = random.choice(dict_css_values)
                for i in xrange(n):
                    s += a + ","
            else:
                for i in xrange(n):
                    s += random.choice(dict_css_values) + ","
        else:
            return random.choice(dict_css_values)

    def fuzz_cssselectorsfunc(self):
        w = random.randint(0, 6)
        if w == 0:
            return "odd"
        elif w == 1:
            return "even"
        elif w == 3:
            return fuzz_randstring()
        elif w == 4:
            return fuzz_xmlattr()
        elif w == 5:
            return str(random.randint(0, 0xFFFFFFFF+1)) + 'n+' + str(random.randint(0, 0xFFFFFFFF+1))
        elif w == 6:
            return str(random.randint(0, 0xFFFFFFFF+1))
        return "foo"

    def fuzz_cssselectors(self):
        s = ""
        if random.randint(0, 2) == 1:
            return ""
        w = random.randint(0, 2)
        if w == 0:
            # simple selector
            return ':' + random.choice(dict_css_selectors)
        elif w == 1:
            # function selector
            return ':' + random.choice(dict_css_func_selectors) + '(' + self.fuzz_cssselectorsfunc() + ')'
        elif w == 2:
            # [attr] [type="text"]
            f = "^$* "
            s += '[' + str(fuzz_xmlattr()) + str(random.choice(f)) + '=' + str(fuzz_randstring()) + ']'
            if random.randint(0, 5) == 3:
                s += ':' + random.choice(dict_css_selectors)
            elif random.randint(0, 5) == 3:
                s += ':' + random.choice(dict_css_func_selectors) + '(' + self.fuzz_cssselectorsfunc() + ')'
            return s

class XMLGen:

    """
    Generate fuzzed SVG based on DTD
    """
    xml_types = {}
    xml_types["svg11"] = {"header": """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/TR/SVG/DTD/svg11.dtd">\n""", "root": """<svg xmlns="http://www.w3.org/2000/svg" version="1.1">\n""", "endroot": """</svg>\n""" }
    xml_types["svg10"] = {"header": """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/SVG/DTD/svg10.dtd">\n""", "root": """<svg xmlns="http://www.w3.org/2000/svg" version="1.0">\n""", "endroot": """</svg>\n""" }
    xml_types["xhtml1"] = {"header": """<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">""", "root": """<html><head><style type="text/css">%(css)s</style></head>""", "endroot": "</html>" }

    def __init__(self, dtd, xmltype="svg11"):
        self.elems = {}
        p = dtdparser.DTDParser()
        p.set_dtd_consumer(DTDConsume(self.elems))
        p.parse_resource(dtd)
        self.toend = []
        self.xmlt = xmltype
        if xmltype == "xhtml1":
            self.css = CSSFuzz(dtd)
        else:
            self.css = None

    def fuzz(self):
        body = random.randint(0, 3)
        nelem = random.randint(1, 10)
        elems = []
        data = ""
        if body:
            data += "<body>"
        for i in xrange(0, nelem):
            # pick random elem
            elem = random.choice(self.elems.keys())
            elems.append(elem)
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
        if body:
            data += "</body>"
        if self.css:
            htmldata = self.xml_types[self.xmlt]["header"]
            htmldata += self.xml_types[self.xmlt]["root"] % {"css":self.css.fuzz(elems)}
        else:
            htmldata = self.xml_types[self.xmlt]["header"] + self.xml_types[self.xmlt]["root"]
        htmldata += data
        htmldata += self.xml_types[self.xmlt]["endroot"]
        return htmldata

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
