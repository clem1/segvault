/*
bf3/rand.c
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
bf3/rand.c
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
#include "include/random.h"
#include "include/mod/css.h"
#include "include/mod/dom.h"
#include "include/mod/html.h"
#include "include/mod/js.h"
#include "include/mod/xml.h"

void rrandom(char *outp, int mod, int mode, int rdn, int rsz, char *mfn1, char *mfn2, int mut)
{

char *tag = NULL, *attr = NULL, *func = NULL, *obj = NULL, *xatt = NULL;
int i, ii, n, tagn, tatt = 0;
struct timeval time;

for(i = 0; i < rdn; i++)
{

int r0 = 0, r1 = 0, r2 = 0, r3 = 0, r4 = 0, r5 = 0, r6 = 0, r7 = 0, r8 = 0, r9 = 0;
int chr = 0, x = 0, y = 0, o = 0;

     gettimeofday(&time, NULL);
     srand(time.tv_sec ^ time.tv_usec);

while(r0 <= 0) r0 = rand() % RNDTL;
while(r1 <= 0) r1 = rand() % RNDTL;
while(r2 <= 0) r2 = rand() % RNDTL;
while(r3 <= 0) r3 = rand() % RNDTL;
while(r4 <= 0) r4 = rand() % RNDTL;
while(r5 <= 0) r5 = rand() % RNDTL;
while(r6 <= 0) r6 = rand() % RNDTL;
while(r7 <= 0) r7 = rand() % RNDTL;
while(r8 <= 0) r8 = rand() % RNDTL;
while(r9 <= 0) r9 = rand() % RNDTL;

while(chr <= 0) chr = rand() % CHRMAX;
while(x <= 0)     x = rand() % rsz+1;

if(mod == 1)
{

while(o <= 0) o = rand() % CSSO;

}

if((mod == 2) && (mode == 1))
{

while(o < DOMO) o = rand() % DOMO+DOMA;

while(y <= 0) y = rand() % DOMATL+1;
     obj = domobja[y];

}

if((mod == 2) && (mode == 2))
{

while(o <= 0) o = rand() % DOMO;

if(o == 5)
{

while(y <= 0) y = rand() % DOM1TL+2;
     obj = domobj1[y];

}

if(o == 6)
{

while(y <= 0) y = rand() % DOM2TL+2;
     obj = domobj2[y];

}

if(o == 7)
{

while(y <= 0) y = rand() % DOM3TL+2;
     obj = domobje[y];

}
}

if(mod == 3)
{

while(o <= 0) o = rand() % HTMLO;
while(y <= 0) y = rand() % HTMLTL+1;

tag  = htmltag[y];
tagn = y;
tatt = htmltagn[y];

}

if((mod == 4) && (mode == 1))
{

while(o < JSO) o = rand() % JSO+JSSA;
while(y <= 0) y = rand() % JSATL+1;

func = jsfunca[y];

}

if((mod == 4) && (mode == 2))
{

while(o <= 0) o = rand() % JSO;
while(y <= 0) y = rand() % JSTL+2; // make sure all of them get a chance to be included in rand()

func = jsfunc[y];

}

if(mod == 5)
{

while(o <= 0) o = rand() % XMLO;

if(o == 14)
{

while(y <= 0) y = rand() % XMLTL+1;
     xatt = xmlssa[y];

}
}

char rnd[x];

     memset(rnd, 0, sizeof(rnd));

if(chr == 1)
{
     strcat(rnd, z[r0]);
}

if(chr == 2)
{
     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);

}

if(chr == 3)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);

}

if(chr == 4)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);

}

if(chr == 5)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);

}

if(chr == 6)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);
     strcat(rnd, ",");
     strcat(rnd, z[r5]);

}

if(chr == 7)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);
     strcat(rnd, ",");
     strcat(rnd, z[r5]);
     strcat(rnd, ",");
     strcat(rnd, z[r6]);

}

if(chr == 8)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);
     strcat(rnd, ",");
     strcat(rnd, z[r5]);
     strcat(rnd, ",");
     strcat(rnd, z[r6]);
     strcat(rnd, ",");
     strcat(rnd, z[r7]);

}

if(chr == 9)
{

     strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);
     strcat(rnd, ",");
     strcat(rnd, z[r5]);
     strcat(rnd, ",");
     strcat(rnd, z[r6]);
     strcat(rnd, ",");
     strcat(rnd, z[r7]);
     strcat(rnd, ",");
     strcat(rnd, z[r8]);

}

if(chr == 10)
{

    strcat(rnd, z[r0]);
     strcat(rnd, ",");
     strcat(rnd, z[r1]);
     strcat(rnd, ",");
     strcat(rnd, z[r2]);
     strcat(rnd, ",");
     strcat(rnd, z[r3]);
     strcat(rnd, ",");
     strcat(rnd, z[r4]);
     strcat(rnd, ",");
     strcat(rnd, z[r5]);
     strcat(rnd, ",");
     strcat(rnd, z[r6]);
     strcat(rnd, ",");
     strcat(rnd, z[r7]);
     strcat(rnd, ",");
     strcat(rnd, z[r8]);
     strcat(rnd, ",");
     strcat(rnd, z[r9]);


}

if(tatt != 0) n = rand() % tatt;

if(mut == 1)
{

     mutate(outp, mfn1, mfn2, rnd, x, mod);

}

if((mod == 1) && (mut == 0))
{

     css_gen(outp, rnd, x, o);

}

if((mod == 2) && (mut == 0))
{

     dom_gen(outp, obj, rnd, x, o);

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

     html_gen(outp, tag, attr, rnd, x, o);

}

if((mod == 4) && (mut == 0))
{

     js_gen(outp, func, rnd, x, o);

}

if((mod == 5) && (mut == 0))
{

     xml_gen(outp, xatt, rnd, x, o);

}
}
}
