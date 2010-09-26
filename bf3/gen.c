/*
bf3/gen.c
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
bf3/gen.c
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

#include "include/core.h"
#include "include/oracle.h"
#include "include/mod/css.h"
#include "include/mod/dom.h"
#include "include/mod/html.h"
#include "include/mod/js.h"
#include "include/mod/xml.h"

void generation(char *outp, char *tag, char *mfn2, int tagn, int tatt, int mod, int o, int mut) // redo variable names to be more generic
{

char *attr, *func = tag, *obj = tag, *xatt = tag, *mfn1 = tag;
int i, ii, n;

     fzof_gen();

for(i = 0; i < tatt; i++)
{

for(ii = 0; ii <= FZTL; ii++)
{

int x = strlen(fuzz[ii].data);
char data[x+256], fn[128];

n = i;

if(mut == 1)
{

     mutate(outp, mfn1, mfn2, fuzz[ii].data, x, mod);

}

if((mod == 1) && (mut == 0))
{

     css_gen(outp, fuzz[ii].data, x, o);

}

if((mod == 2) && (mut == 0))
{

     dom_gen(outp, obj, fuzz[ii].data, x, o);

}

if((mod == 3) && (mut == 0))
{

if(tagn == 1)  attr = body[n];
if(tagn == 2)  attr = p[n];
if(tagn == 3)  attr = hr[n];
if(tagn == 4)  attr = font[n];
if(tagn == 5)  attr = bdo[n];
if(tagn == 6)  attr = pre[n];
if(tagn == 7)  attr = a[n];
if(tagn == 8)  attr = llink[n];
if(tagn == 9)  attr = frame[n];
if(tagn == 10) attr = frameset[n];
if(tagn == 11) attr = iframe[n];
if(tagn == 12) attr = form[n];
if(tagn == 13) attr = input[n];
if(tagn == 14) attr = textarea[n];
if(tagn == 15) attr = button[n];
if(tagn == 16) attr = sselect[n];
if(tagn == 17) attr = optgroup[n];
if(tagn == 18) attr = option[n];
if(tagn == 19) attr = label[n];
if(tagn == 20) attr = legend[n];
if(tagn == 21) attr = ul[n];
if(tagn == 22) attr = ol[n];
if(tagn == 23) attr = li[n];
if(tagn == 24) attr = dir[n];
if(tagn == 25) attr = menu[n];
if(tagn == 26) attr = img[n];
if(tagn == 27) attr = map[n];
if(tagn == 28) attr = area[n];
if(tagn == 29) attr = table[n];
if(tagn == 30) attr = caption[n];
if(tagn == 31) attr = th[n];
if(tagn == 32) attr = tr[n];
if(tagn == 33) attr = td[n];
if(tagn == 34) attr = thead[n];
if(tagn == 35) attr = tbody[n];
if(tagn == 36) attr = tfoot[n];
if(tagn == 37) attr = col[n];
if(tagn == 38) attr = colgroup[n];
if(tagn == 39) attr = style[n];
if(tagn == 40) attr = ddiv[n];
if(tagn == 41) attr = head[n];
if(tagn == 42) attr = meta[n];
if(tagn == 43) attr = base[n];
if(tagn == 44) attr = basefont[n];
if(tagn == 45) attr = script[n];
if(tagn == 46) attr = applet[n];
if(tagn == 47) attr = object[n];
if(tagn == 48) attr = param[n];

     html_gen(outp, tag, attr, fuzz[ii].data, x, o);

}

if((mod == 4) && (mut == 0))
{

     js_gen(outp, func, fuzz[ii].data, x, o);

}

if((mod == 5) && (mut == 0))
{

     xml_gen(outp, attr, fuzz[ii].data, x, o);

}
}
}
}
