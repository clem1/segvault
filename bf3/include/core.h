/*
bf3/include/core.h
01.16.2010
bf3

         Krakow Labs Development -> bf3
                Browser Fuzzer 3
                   jbrown@KL

fuzz@krakowlabs:~# bf3.tar.gz

Associated Files & Information:
http://www.krakowlabs.com/dev/fuz/bf3/bf3
http://www.krakowlabs.com/dev/fuz/bf3/bf3.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/cli.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/core.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/gen.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mut.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/oracle.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/rand.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mod/css.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mod/dom.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mod/html.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mod/js.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/mod/xml.c.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/core.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/oracle.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/random.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/mod/css.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/mod/dom.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/mod/html.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/mod/js.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/include/mod/xml.h.txt
http://www.krakowlabs.com/dev/fuz/bf3/Makefile.txt
http://www.krakowlabs.com/dev/fuz/bf3/media/bf3.jpeg
http://www.krakowlabs.com/dev/fuz/bf3/media/bf3.avi
http://www.krakowlabs.com/dev/fuz/bf3/samples/css/bmgsec.html.txt
http://www.krakowlabs.com/dev/fuz/bf3/samples/css/style.css.txt
http://www.krakowlabs.com/dev/fuz/bf3/samples/xml/index.html.txt
http://www.krakowlabs.com/dev/fuz/bf3/samples/xml/cd_catalog.xml.txt
http://www.krakowlabs.com/dev/fuz/bf3/doc/bf3_doc.txt
http://www.krakowlabs.com/dev/fuz/bf3/bf3.tar.gz
bf3/include/core.h
*/

/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#define BANNER  "\n         Krakow Labs Development -> bf3\n                Browser Fuzzer 3\n                   jbrown@KL\n"
#define OUTPDIR "/var/www"

#define MODTL    5
#define PORT  2000
#define SIZE  1024

#define RNDSZ     100000
#define RNDTL         (sizeof(z)/sizeof(char *))
#define RNDMAX 100000000
#define CHRMAX      10+1

#define HTTPRESP "200 HTTP/1.1 OK\r\nServer: Browser Fuzzer 3\r\n\r\n"

#define REFRESHA "<head><meta http-equiv=\"refresh\" content=\"1; url="
#define REFRESHB "\"></head>"
#define REFRESHC "\">"

#define TESTDATA "fuzz"

int engine(char *outp, int mod, int mode, int fzo, int rdn, int rsz, char *mfn1, char *mfn2, int mut);

void usage(char *prog);
void counter(int *n);
void fzof_gen(void);

void list_modules(void);
void list_oracle(void);

void generation(char *outp, char *tag, char *mfn2, int tagn, int tatt, int mod, int o, int mut);
void rrandom(char *outp, int mod, int mode, int rdn, int rsz, char *mfn1, char *mfn2, int mut);

void mutate(char *outp, char *mfn1, char *mfn2, char *fuzz, int x, int mod);

void css(char *outp, int mod, int fzo, int rdn, int rsz);
void dom(char *outp, int mod, int mode, int fzo, int rdn, int rsz);
void html(char *outp, int mod, int fzo, int rdn, int rsz);
void js(char *outp, int mod, int mode, int fzo, int rdn, int rsz);
void xml(char *outp, int mod, int fzo, int rdn, int rsz);

void css_gen(char *outp, char *fuzz, int x, int o);
void dom_gen(char *outp, char *obj, char *fuzz, int x, int o);
void html_gen(char *outp, char *tag, char *attr, char *fuzz, int x, int o);
void js_gen(char *outp, char *func, char *fuzz, int x, int o);
void xml_gen(char *outp, char *xatt, char *fuzz, int x, int o);

static int cnt = 0;
