#!/usr/bin/env python
from BeautifulSoup import BeautifulSoup as bs
from BeautifulSoup import NavigableString as ns
import random
import copy

def bsfuzzer_attr():
    what = random.randint(0, 18)
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
        return ((str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + ",")*random.randint(1,500))[:-1]
    elif what == 11:
        return str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "%"
    elif what == 12:
        return str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "px"
    elif what == 13:
        return str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "em"
    elif what == 14:
        return ((str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "." + (str(random.randint(-0xFFFFFFFF, 0xFFFFFFF))) + ",")*random.randint(1,500))[:-1]
    elif what == 15:
        return ((str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "." + (str(random.randint(-0xFFFFFFFF, 0xFFFFFFF))) + " ")*random.randint(1,500))[:-1]
    elif what == 16:
        return str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) + "." + (str(random.randint(-0xFFFFFFFF, 0xFFFFFFF))) + "%"
    elif what == 17:
        return str(random.randint(-0xFFFFFFFF, 0xFFFFFFF)) +"." + (str(random.randint(-0xFFFFFFFF, 0xFFFFFFF))) + "px"
    return "f000"

def bsfuzzer_data(data):
    what = random.randint(0, 5)
    if what == 0:
        return "A"*random.randint(1, 0x7FF)
    elif what == 1:
        return "%x"*random.randint(0,500)+"%n"*random.randint(0,100)
    elif what == 2:
        return str(chr(random.randint(0, 255))) * random.randint(0,100)
    elif what == 3:
        return "fuck#@%$#@~#~^..Dqqdksqo%FF%FF\uFF"
    elif what == 4:
        idx = random.randint(0, len(data)-1)
        return data[0:idx] + data[idx] * random.randint(0, 50000) + data[idx+1:]
    return "f000"

class HTMLFuzzer:

    def __init__(self, data):
        self.soup = bs(data)

    def fuzz(self):
        f = copy.copy(self.soup)
        for ms in f.findAll()[1:]:
            # fuzzing attributes.
            if random.randint(0, 5) == 1 and ms.attrs:
                attrs = []
                for attr in ms.attrs:
                    if random.randint(0, 4) == 1:
                        attrs.append((attr[0], bsfuzzer_attr()))
                    else:
                        attrs.append(attr)
                ms.attrs = attrs
            # fuzzing data.
            if random.randint(0, 8) == 1 and len(ms.contents) == 1:
                try:
                    ms.contents[0] = ns(bsfuzzer_data(str(ms.contents[0])))
                except:
                    pass
        return str(f)

if __name__ == "__main__":
    print BSFuzzer("""<xml attr1="a" haha="a" jdijdi="oqjiq" jdijd="jkdiqjdi" odqo="jdiqjd"><a>f00!</a><b coin="111"></b></xml>""").fuzz()

