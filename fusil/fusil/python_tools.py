from sys import version, hexversion

RUNNING_PYPY = ("pypy" in version.lower())
RUNNING_PYTHON3 = (hexversion >= 0x03000000)

