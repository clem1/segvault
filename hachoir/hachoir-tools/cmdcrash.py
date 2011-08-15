'''
Simple cmd crash peach published

@author: clem1
'''

import sys
import md5

from Peach.publisher import Publisher
from app_unix import Application

# After each fuzzed file is served, a copy of it will be stored in this file.
PROG = "clamscan"
FUZZDIR = "/tmp/fuzz/"
NUMFUZZ = 500

class CmdCrashPublisher(Publisher):
    '''
    Each round of generation will result in the following calls:
    start
    connect
    send
    close
    stop
    '''

    def __init__(self):
        # Definining withNode prevents some peach error.
        self.withNode = False
        self.num = 0

    def _cmdcrash(self):
        app = Application(PROG, (FUZZDIR,))
        app.start()
        try:
            app._wait(True)
        except KeyboardInterrupt:
            print "Interrupt!"
            app.stop()
            return True
        return app.exit_failure and app.exit_code is None

    def start(self):
        pass

    def connect(self):
        pass

    def send(self, data):
        '''Peach calls this to provide us the fuzzer-generated data.'''
        f = open("%s/fuzz-%d.zip" % (FUZZDIR, self.num), "w")
        if f:
            print "datalen: %d, fuzzedmd5: %s" % (len(data), md5.new(data).hexdigest())
            self.num += 1
            f.write(data)
            f.close()

        if self.num == NUMFUZZ:
            print "%d iters, launching %s" % (self.num, PROG)
            if self._cmdcrash():
                print "Found one success crash"
                sys.exit(1)
            self.num = 0

    def close(self):
        pass

    def stop(self):
        pass

    def property(self, property, value = None):
        pass
