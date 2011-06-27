from fusil.mangle import MangleFile, MangleXML
from fusil.tools import minmax
from array import array

class AutoMangle(MangleFile):
    def __init__(self, project, *args, **kw):
        MangleFile.__init__(self, project, args[0], int(kw['nb_file']))
        self.hard_max_op = 10000
        self.hard_min_op = 0
        self.aggressivity = None
        self.fixed_size_factor = 1.0
        if kw.has_key('ext'):
            if kw['ext'] in ('.xml', '.svg', '.rdf'):
                if kw.has_key('nofile') and kw['nofile']:
                    self.xml = MangleXML(nofile=True, xmltype=MangleXML.SVG11)
                else:
                    self.xml = MangleXML(xmltype=MangleXML.SVG11)
            elif kw['ext'] == ".html":
                if kw.has_key('nofile'):
                    self.xml = MangleXML(nofile=kw['nofile'], xmltype=MangleXML.XHTML1)
                else:
                    self.xml = MangleXML(xmltype=MangleXML.XHTML1)
        else:
            self.xml = None

    def on_session_start(self):
        pass

    def on_aggressivity_value(self, value):
        self.aggressivity = value
        self.mangle()

    def setupConf(self, data):
        operations = ["bit"]
        size_factor = 0.30

        if 0.25 <= self.aggressivity:
            operations.append("increment")
        if 0.30 <= self.aggressivity:
            operations.extend(("replace", "special_value"))
        if 0.50 <= self.aggressivity:
            operations.extend(("insert_bytes", "delete_bytes"))
            size_factor = 0.20
        self.config.operations = operations

        # Display config
        count = len(data) * size_factor * self.fixed_size_factor
        count = minmax(self.hard_min_op, count, self.hard_max_op)
        count = int(count * self.aggressivity)
        self.config.max_op = max(count, self.hard_min_op)
        self.config.min_op = max(int(self.config.max_op * 0.80), self.hard_min_op)
        self.warning("operation#:%s..%s  operations=%s"
            % (self.config.min_op, self.config.max_op, self.config.operations))

    def mangleData(self, data, file_index):
        self.setupConf(data)
        if self.xml:
            return array('B', self.xml.mangleData(data.tostring()))
        return MangleFile.mangleData(self, data, file_index)

