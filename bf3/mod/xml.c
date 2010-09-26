/*
bf3/mod/xml.c
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
bf3/mod/xml.c
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
#include "../include/mod/xml.h"
#include "../include/mod/html.h"

void xml(char *outp, int mod, int fzo, int rdn, int rsz)
{

char *xatt = NULL;
int i;

if(fzo == 1)
{

     generation(outp, NULL, NULL, 0, 1, mod, 1, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 2, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 3, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 4, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 5, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 6, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 7, 0);
     generation(outp, NULL, NULL, 0, 1, mod, 8, 0);
	generation(outp, NULL, NULL, 0, 1, mod, 9, 0);
	generation(outp, NULL, NULL, 0, 1, mod, 10, 0);
	generation(outp, NULL, NULL, 0, 1, mod, 11, 0);
	generation(outp, NULL, NULL, 0, 1, mod, 12, 0);
	generation(outp, NULL, NULL, 0, 1, mod, 13, 0);

for(i = 0; i <= XMLTL; i++)
{

xatt = xmlssa[i];

     generation(outp, xatt, NULL, 0, 1, mod, 14, 0);

}
}

if(fzo == 2)
{

     rrandom(outp, mod, 0, rdn, rsz, NULL, NULL, 0);

}
}

void xml_gen(char *outp, char *xatt, char *fuzz, int x, int o)
{

char data[x*2+512], fn[128], *modn = "xml";

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     counter(&cnt);

     snprintf(fn, sizeof(fn), "%s/%s%d.html", outp, modn, cnt);

     snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s%s%d%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, XMLB, modn, cnt, XMLC, HTMLB);

     cli_eng(data, fn);

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     snprintf(fn, sizeof(fn), "%s/%s%d.%s", outp, modn, cnt, modn);

if(o == 1)
     snprintf(data, sizeof(data), "%s %s %s", XMLDATAP, fuzz, XMLDATAQ);

if(o == 2)
     snprintf(data, sizeof(data), "%s%s%s", XMLDATAR, fuzz, XMLDATAT);

if(o == 3)
     snprintf(data, sizeof(data), "%s%s%s", XMLDATAS, fuzz, XMLDATAT);

if(o == 4)
     snprintf(data, sizeof(data), "%s\n%s%s%s\n%s%s%s", XMLA, XMLDATAA, fuzz, XMLDATAB, XMLDATAC, fuzz, XMLDATAB);

if(o == 5)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s%s%s%s%s\n%s", XMLA, XMLDATAD, XMLDATAA, fuzz, XMLDATAB, XMLDATAC, fuzz, XMLDATAB, XMLDATAE);

if(o == 6)
     snprintf(data, sizeof(data), "%s\n%s\n%s %s %s%s\n%s", XMLA, XMLDATAD, XMLDATAF, fuzz, XMLDATAO, XMLDATAI, XMLDATAE);

if(o == 7)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s%s%s\n%s", XMLA, XMLDATAD, XMLDATAG, fuzz, XMLDATAN, XMLDATAI, XMLDATAE);

if(o == 8)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s%s\n%s", XMLA, XMLDATAD, XMLDATAH, fuzz, XMLDATAI, XMLDATAE);

if(o == 9)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s\n%s%s%s%s%s%s\n%s\n%s", XMLA, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAA, fuzz, XMLDATAB, XMLDATAC, fuzz, XMLDATAB, XMLDATAI, XMLDATAE);

if(o == 10)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s\n%s %s %s%s\n%s\n%s", XMLA, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAJ, fuzz, XMLDATAO, XMLDATAM, XMLDATAI, XMLDATAE);

if(o == 11)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s\n%s%s%s%s\n%s\n%s", XMLA, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAK, fuzz, XMLDATAN, XMLDATAM, XMLDATAI, XMLDATAE);

if(o == 12)
     snprintf(data, sizeof(data), "%s\n%s\n%s%s\n%s%s%s\n%s\n%s", XMLA, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAL, fuzz, XMLDATAM, XMLDATAI, XMLDATAE);

if(o == 13)
     snprintf(data, sizeof(data), "%s\n%s %s %s\n%s\n%s%s\n%s%s%s%s\n%s\n%s", XMLA, XMLDATAX, fuzz, XMLDATAQ, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAJ, XMLDATAB, XMLDATAU, XMLDATAM, XMLDATAI, XMLDATAE);

if(o == 14)
{

if(strcmp(xatt, "href") == 0)
     snprintf(data, sizeof(data), "%s\n%s %s %s%s%s\n%s\n%s%s\n%s%s%s%s\n%s\n%s", XMLA, XMLDATAW, xatt, XMLDATAY, fuzz, XMLDATAT, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAJ, XMLDATAB, XMLDATAU, XMLDATAM, XMLDATAI, XMLDATAE);

if(strcmp(xatt, "type") == 0)
     snprintf(data, sizeof(data), "%s\n%s %s %s%s%s\n%s\n%s%s\n%s%s%s%s\n%s\n%s", XMLA, XMLDATAV, xatt, XMLDATAY, fuzz, XMLDATAT, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAJ, XMLDATAB, XMLDATAU, XMLDATAM, XMLDATAI, XMLDATAE);

else if((strcmp(xatt, "href") != 0) && (strcmp(xatt, "type") != 0))
     snprintf(data, sizeof(data), "%s\n%s %s %s%s%s\n%s\n%s%s\n%s%s%s%s\n%s\n%s", XMLA, XMLDATAX, xatt, XMLDATAY, fuzz, XMLDATAT, XMLDATAD, XMLDATAF, XMLDATAB, XMLDATAJ, XMLDATAB, XMLDATAU, XMLDATAM, XMLDATAI, XMLDATAE);

}

     cli_eng(data, fn);

}
