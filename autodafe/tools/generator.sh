#!/bin/sh
# script used to create the fuzz dictionary. Basic functions.
# need perl (famous perl -e '...')
# 
# Copyright (c) 2004 Martin Vuagnoux <autodafe@vuagnoux.com>

help_usage()
{
    echo "$0: creation of the fuzz dictionary"
    echo "Copyright (c) 2004 Martin Vuagnoux <autodafe@vuagnoux.com>"
    echo "USAGE  : $0 <dir_to_install_autodafe_fuzz_dic>"
    echo "EXAMPLE: $0 /usr/local/etc/"
    exit 0
}

# test the arguments
#if [ "$1" == "" ]
#    then
#    help_usage
#fi

# test if the directory exist
if [ -d $1 ]
    then
    echo "[*] creating directory: ./autodafe"
    mkdir ./autodafe 2> /dev/null
    mkdir ./autodafe/string 2> /dev/null
    mkdir ./autodafe/hex 2> /dev/null
    rm -f ./autodafe/string.fuzz 2> /dev/null
    rm -f ./autodafe/hex.fuzz 2> /dev/null

    echo "[*] creating file: ./autodafe/string.fuzz"
    touch ./autodafe/string.fuzz

    echo "#============================================================="\
    >> ./autodafe/string.fuzz
    echo "# Copyright 2004 (C) Martin Vuagnoux <autodafe@vuagnoux.com>  "\ >> ./autodafe/string.fuzz
    echo "# STRING FUZZ BASIC DICTIONARY                                "\
    >> ./autodafe/string.fuzz
    echo "#============================================================="\
    >> ./autodafe/string.fuzz
    echo "# WARNING! Each file here should not contain '\x0a' or \x0d' "\
    >> ./autodafe/string.fuzz
    echo "#============================================================="\
    >> ./autodafe/string.fuzz
    echo "[*] creating file: ./autodafe/hex.fuzz"
    touch ./autodafe/hex.fuzz

    echo "#============================================================="\
    >> ./autodafe/hex.fuzz
    echo "# Copyright 2004 (C) Martin Vuagnoux <autodafe@vuagnoux.com>  "\ >> ./autodafe/hex.fuzz
    echo "# HEX FUZZ BASIC DICTIONARY                                   "\
    >> ./autodafe/hex.fuzz
    echo "#============================================================="\
    >> ./autodafe/hex.fuzz


    # repetion basic strings
    counter=0
    for i in   \
	"a"    \
	"d"    \
	"A"    \
	"D"    \
        "%s"   \
        "%n"   \
        ">"    \
        "<"    \
        "("    \
        ")"    \
        "/"    \
        "&"    \
        ";"    \
        "|"    \
        "\\"    \
	
      do
      echo -ne "[*] processing: \"$i\"\t\t 0%["
      counter=`expr $counter + 1`
      for j in 3 4 15 16 31 32 63 64 127 128 255 256 511 512 1023 1024 2047 2048 \
	       4095 4096 8191 8192 16383 16384 32767 32768 65535 65536 99999
#      for j in 99999 65536 65535 32768 32767 16384 16483 8192 8191 4096 4095 \
#               2048 2047 1024 1023 512 511 256 255 128 127 64 63 32 31 16 15 4 3
      do
	./overflow  $j "$i" > ./autodafe/string/string-$counter-x$j

	# space
	echo -n " " > ./autodafe/string/string-$counter-sp-x$j
	./overflow  $j "$i" >> ./autodafe/string/string-$counter-sp-x$j

	# >
	echo -n ">" > ./autodafe/string/string-$counter-geq-x$j
	./overflow  $j "$i" >> ./autodafe/string/string-$counter-sp-x$j

	# <
	echo -n "<" > ./autodafe/string/string-$counter-leq-x$j
	./overflow  $j "$i" >> ./autodafe/string/string-$counter-sp-x$j

	# ;
	echo -n ";" > ./autodafe/string/string-$counter-leq-x$j
	./overflow  $j "$i" >> ./autodafe/string/string-$counter-sp-x$j

	echo -e "$1/autodafe/string/string-$counter-x$j \t# \"$i\" x $j" >> ./autodafe/string.fuzz
	echo -e "$1/autodafe/string/string-$counter-sp-x$j \t# <space>\"$i\" x $j" >> ./autodafe/string.fuzz
	echo -e "$1/autodafe/string/string-$counter-geq-x$j \t# \">\" \"$i\" x $j" >> ./autodafe/string.fuzz
	echo -e "$1/autodafe/string/string-$counter-leq-x$j \t# \"<\" \"$i\" x $j" >> ./autodafe/string.fuzz
	echo -e "$1/autodafe/string/string-$counter-leq-x$j \t# \";\" \"$i\" x $j" >> ./autodafe/string.fuzz
	echo -n "."
      done;
      echo "]100%"
    done;
fi
