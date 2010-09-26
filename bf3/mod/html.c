/*
bf3/mod/html.c
01.15.2010
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
bf3/mod/html.c
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

#include "../include/core.h"
#include "../include/oracle.h"
#include "../include/mod/html.h"

void html(char *outp, int mod, int fzo, int rdn, int rsz)
{

char *tag = NULL;
int i, tagn, tatt;

if(fzo == 1)
{

     generation(outp, tag, NULL, 0, 1, mod, 1, 0);
     generation(outp, tag, NULL, 0, 1, mod, 2, 0);

for(i = 0; i <= HTMLTL; i++)
{

tag  = htmltag[i];
tagn = i+1;
tatt = htmltagn[i];

     generation(outp, tag, NULL, tagn, tatt, mod, 3, 0);
     generation(outp, tag, NULL, tagn, tatt, mod, 4, 0);

}
}

if(fzo == 2)
{

     rrandom(outp, mod, 0, rdn, rsz, NULL, NULL, 0);

}
}

void html_gen(char *outp, char *tag, char *attr, char *fuzz, int x, int o)
{

char data[x*2+512], fn[128], *modn = "html";

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     counter(&cnt);

     snprintf(fn, sizeof(fn), "%s/%s%d.html", outp, modn, cnt);

if(o == 1)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n<%s %s = \"%s\">\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, fuzz, HTMLDATAB, TESTDATA, HTMLB);

if(o == 2)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n<%s %s = \"%s\">\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, HTMLDATAA, fuzz, TESTDATA, HTMLB);

if(o == 3)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n<%s %s = \"%s\">\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, tag, attr, fuzz, HTMLB);

if(o == 4)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n<%s %s = %s>\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, tag, attr, fuzz, HTMLB);

     cli_eng(data, fn);

}
