# -*- coding: cp1252 -*-
import socket, thread, select, urllib2, re, random, os, os.path
from htmlfuzz import HTMLFuzzer
from mangle import mangle
from fuzznumber import FuzzNumber
from urlparse import urlparse

__version__ = '0.1.0'
BUFLEN = 8192
VERSION = 'Froxy/'+__version__
HTTPVER = 'HTTP/1.1'

SERVER_HEADER = """HTTP/1.1 200 OK
Server: FroxyWeb
Content-Type: %(mime)s
Connection: close

"""

HEADER_404 = """HTTP/1.1 404 Not Found
Server: FroxyWeb
Connection: close

"""

class URLCache:

    def __init__(self):
        self.cache = {}

    def hash(self, u):
        o = urlparse(u)
        return o.netloc + "/" + o.path

    def get(self, u):
        try:
            return self.cache[self.hash(u)]
        except:
            return None

    def add(self, u, d):
        self.cache[self.hash(u)] = d

class ConnectionHandler:

    MIMERX = re.compile(".*Content-Type:\s*(.*?)\r?\n.*", re.DOTALL)

    def __init__(self, connection, address, timeout, cache):
        self.fuzzed = False
        self.client = connection
        self.client_buffer = ''
        self.timeout = timeout
        self.cache = cache
        self.method, self.path, self.protocol = self.get_base_header()
        self.handle()
        self.client.close()

    def get_base_header(self):
        while 1:
            self.client_buffer += self.client.recv(BUFLEN)
            end = self.client_buffer.find('\n')
            if end!=-1:
                break
        #print '%s'%self.client_buffer[:end]
        data = (self.client_buffer[:end+1]).split()
        self.client_buffer = self.client_buffer[end+1:]
        return data

    def handle(self):
        self.grabpage(self.path)
        mdata = self.mangledata(self.data)
        if self.fuzzed:
            self.client.send(SERVER_HEADER % {"mime": self.mime})
            self.client.send(mdata)
            self.savedata(self.path, mdata)
        else:
            self.client.send(HEADER_404)

    def savedata(self, url, mdata):
        if self.fuzzed:
            p = self.cache.hash(url)
            try:
                if p[-1] == "/":
                    try:
                        os.makedirs("pages/" + p)
                    except:
                        p = p + "/index.html"
                else:
                    os.makedirs("pages/" + os.path.dirname(p))
            except:
                pass
            l = open("pages/" + p, "w")
            l.write(mdata)
            l.close()

    def addtime(self, data):
        return data
        #return data.replace("<head>", """<head><meta http-equiv="refresh" content="2;url=/">""")

    def grabpage(self, url):
        if self.cache.get(url):
            self.mime, self.data = self.cache.get(url)
            return
        try:
            r = urllib2.urlopen(urllib2.Request(url))
            self.data = r.read()
        except:
            self.mime = "text/html"
            self.data = ""
            return

        try:
            self.mime = self.MIMERX.match(str(r.info()), re.DOTALL).groups()[0]
        except:
            self.mime = "text/html"
        if self.mime.find("html") >= 0:
            self.data = self.addtime(self.data)
        self.cache.add(url, (self.mime, self.data))

    def white(self):
        if self.path.find("google.com/") >= 0:
            return True
        return False

    def mangledata(self, data):
        self.fuzzed = False
        if self.white() or len(data) == 0:
            return data
        if self.mime.find("html") >= 0 or self.mime.find("svg") >= 0 or self.mime.find("xml") >= 0:
            self.fuzzed = True
            w = random.randint(2, 5)
            if w == 3:
                return HTMLFuzzer(data).fuzz()
            elif w == 4:
                return FuzzNumber().fuzz(data)
        elif self.mime.find("css") >= 0:
            self.fuzzed = True
            return FuzzNumber().fuzz(data)
        elif self.mime.find("javascript") >= 0:
            self.fuzzed = True
            return FuzzNumber().fuzz(data)
        #elif self.mime.find("image") >= 0 or self.mime.find("swf") >= 0 or self.mime.find("flash") >= 0 or self.mime.find("audio") >= 0:
        #    self.fuzzed = True
        #    d = bytearray(data)
        #    mangle(d, random.randint(1, 10))
        #    return d
        return data

def start_server(host='localhost', port=8081, IPv6=False, timeout=60,
                  handler=ConnectionHandler):
    if IPv6==True:
        soc_type=socket.AF_INET6
    else:
        soc_type=socket.AF_INET
    soc = socket.socket(soc_type)
    soc.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    soc.bind((host, port))
    print "Serving on %s:%d."%(host, port)
    soc.listen(0)
    cache = URLCache()
    try:
        while 1:
            thread.start_new_thread(handler, soc.accept()+(timeout,cache))
    except:
        soc.close()

if __name__ == '__main__':
    start_server()
