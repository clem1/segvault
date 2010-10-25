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
    else:
        return random.randint(0, 100000)
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
