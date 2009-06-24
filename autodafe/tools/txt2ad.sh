#!/bin/sh
#
# Copyright (C) 2004 Martin Vuagnoux <autodafe@vuagnoux.com>
#
# txt2ad: Convert a text file to an ad file (use string("");)


help_usage() 
{
    echo "$0: convert a txt file to an ad file"
    echo "Copyright (C) 2004 Martin Vuagnoux <autodafe@vuagnoux.com>"
    echo "USAGE  : $0 <txt_file> (ad file is stdout)"
    echo "EXAMPLE: $0 ./text.txt > ./text.ad"
    exit 0
}

# test the argument
if [ "$1" == "" ]
    then
    help_usage
fi

# test if the file exists
if [ -f $1 ]
    then
    echo "block_begin(\"1\");"
    cat $1 | awk '{printf "string(\"%s\"); hex(0a);\n",$0}'
    echo "block_end(\"1\");"
    echo "send(\"1\");"
fi
