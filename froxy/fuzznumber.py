import random

EVILS = [ 0xFF, 0x80, 0x7F, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFF, 0x7FFFFFFF, 0x7FFFFFFFFFFFFFFF, 0x8000, 0x80000000, 0x8000000000000000, 0, -1 ]

class FuzzNumber:
    def __init__(self):
        return

    def isdigit(self, c):
        return c >= '0' and c <= '9'

    def num(self):
        if random.randint(0, 50) < 20:
            return random.randint(0, 0xFFFFFFFF)
        return random.choice(EVILS)

    def fuzz(self, data):
        """look for a number and replace it with an evil one"""
        d = ""
        was_digit = False
        prev_digit = ""
        for c in data:
            if self.isdigit(c):
                was_digit = True
                prev_digit += c
            else:
                if was_digit:
                    if random.randint(0, 10) == 2:
                        d = "%s%lu" % (d, self.num())
                    else:
                        d += prev_digit
                    prev_digit = ""
                    was_digit = False
                d += c
        return d
