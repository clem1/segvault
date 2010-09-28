/*
bf3/mod/js.c
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
bf3/mod/js.c
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
#include "../include/mod/js.h"
#include "../include/mod/html.h"

void js(char *outp, int mod, int mode, int fzo, int rdn, int rsz)
{

char *func = NULL;
int i;

if((mode == 1) && (fzo == 1))
{

for(i = 0; i <= JSATL; i++)
{

func = jsfunca[i];

     generation(outp, func, NULL, 0, 1, mod, 4, 0);
     generation(outp, func, NULL, 0, 1, mod, 5, 0);

}
}

if((mode == 2) && (fzo == 1))
{

     generation(outp, func, NULL, 0, 1, mod, 1, 0);

for(i = 0; i <= JSTL; i++)
{

func = jsfunc[i];

     generation(outp, func, NULL, 0, 1, mod, 2, 0);
     generation(outp, func, NULL, 0, 1, mod, 3, 0);

}
}

if(fzo == 2)
{

     rrandom(outp, mod, mode, rdn, rsz, NULL, NULL, 0);

}
}

void js_gen(char *outp, char *func, char *fuzz, int x, int o)
{

char data[x*2+512], fn[128], *modn = "js";

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     counter(&cnt);

     snprintf(fn, sizeof(fn), "%s/%s%d.html", outp, modn, cnt);

#if 0
if(o == 1)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n%s\n%s(\"%s\");\n%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, JSA, JSDATA, fuzz, TESTDATA, JSB, HTMLB);

if(o == 2)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n%s\n%s(\"%s\");\n%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, JSA, JSDATA, func, fuzz, JSB, HTMLB);

if(o == 3)
#endif
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n%s\n%s(%s);\n%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, JSA, JSDATA, func, fuzz, JSB, HTMLB);

#if 0
if(o == 4)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n%s\n%s(%s, %s);\n%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, JSA, JSDATA, func, fuzz, fuzz, JSB, HTMLB);

if(o == 5)
     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n%s\n%s(\"%s\", \"%s\");\n%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, JSA, JSDATA, func, fuzz, fuzz, JSB, HTMLB);
#endif
     cli_eng(data, fn);

}
