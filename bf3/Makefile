# bf3/Makefile
# 01.16.2010
# bf3
#
#          Krakow Labs Development -> bf3
#                 Browser Fuzzer 3
#                    jbrown@KL
#
# fuzz@krakowlabs:~# bf3.tar.gz
#
# Associated Files & Information:
# http://www.krakowlabs.com/dev/fuz/bf3/bf3
# http://www.krakowlabs.com/dev/fuz/bf3/bf3.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/cli.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/core.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/gen.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mut.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/oracle.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/rand.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mod/css.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mod/dom.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mod/html.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mod/js.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/mod/xml.c.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/core.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/oracle.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/random.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/mod/css.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/mod/dom.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/mod/html.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/mod/js.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/include/mod/xml.h.txt
# http://www.krakowlabs.com/dev/fuz/bf3/Makefile.txt
# http://www.krakowlabs.com/dev/fuz/bf3/media/bf3.jpeg
# http://www.krakowlabs.com/dev/fuz/bf3/media/bf3.avi
# http://www.krakowlabs.com/dev/fuz/bf3/samples/css/bmgsec.html.txt
# http://www.krakowlabs.com/dev/fuz/bf3/samples/css/style.css.txt
# http://www.krakowlabs.com/dev/fuz/bf3/samples/xml/index.html.txt
# http://www.krakowlabs.com/dev/fuz/bf3/samples/xml/cd_catalog.xml.txt
# http://www.krakowlabs.com/dev/fuz/bf3/doc/bf3_doc.txt
# http://www.krakowlabs.com/dev/fuz/bf3/bf3.tar.gz
# bf3/Makefile

CC = gcc
#CFLAGS = -static
#CFLAGS = -ggdb # debug
bf3: bf3.c core.c oracle.c gen.c rand.c mut.c cli.c mod/css.c mod/dom.c mod/html.c mod/js.c mod/xml.c
clean:
	rm -f bf3
