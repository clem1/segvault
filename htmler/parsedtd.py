from xml.parsers.xmlproc import dtdparser, xmlapp
import sys

class DTDConsume(xmlapp.DTDConsumer):

    def __init__(self, elems, attrs):
        self.attrs = attrs
        self.elems = elems

    def new_attribute(self, elem, attr, a_type, a_decl, a_def):
        self.elems.append(elem)
        self.attrs.append(attr)

def parsedtd(infile):
    elems = []
    attrs = []
    p = dtdparser.DTDParser()
    p.set_dtd_consumer(DTDConsume(elems, attrs))
    p.parse_resource(infile)
    return (elems, attrs)

(e, a) = parsedtd(sys.argv[1])
for ee in set(e):
    print """'%s',""" % ee

print
print

for aa in set(a):
    print """'%s',""" % aa
