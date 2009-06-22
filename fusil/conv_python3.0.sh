#!/bin/bash
set -e
set -x
2to3 -w . fuzzers/fusil-* fuzzers/notworking/fusil-*
2to3 -w -d doc/*.rst tests/*.rst
patch -p1 < python3.0.patch
