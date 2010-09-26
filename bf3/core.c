/*
bf3/core.c
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
bf3/core.c
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

int engine(char *outp, int mod, int mode, int fzo, int rdn, int rsz, char *mfn1, char *mfn2, int mut)
{

     printf("%s\n", BANNER);
//printf("outp=%s mod=%d mode=%d fzo=%d rdn=%d rsz=%d mut=%d\n", outp, mod, mode, fzo, rdn, rsz, mut);
if((mut == 1) && (fzo == 1)) generation(outp, mfn1, mfn2, 0, 1, mod, 0, mut);
if((mut == 1) && (fzo == 2)) rrandom(outp, mod, mode, rdn, rsz, mfn1, mfn2, mut);

if((mod == 1) && (mut == 0)) css(outp, mod, fzo, rdn, rsz);
if((mod == 2) && (mut == 0)) dom(outp, mod, mode, fzo, rdn, rsz);
if((mod == 3) && (mut == 0)) html(outp, mod, fzo, rdn, rsz);
if((mod == 4) && (mut == 0)) js(outp, mod, mode, fzo, rdn, rsz);
if((mod == 5) && (mut == 0)) xml(outp, mod, fzo, rdn, rsz);

     return;

}

void counter(int *n)
{

     (*n)++;

}

void list_oracle(void)
{

int i;

     printf("%s", BANNER);
     printf("\n               [Fuzzing Oracle]\n\n");

for(i = 0; i <= FZTL; i++)
{

     printf("[%d] %s\n", i+1, fuzz[i].desc);

}

     printf("\n");
     exit(0);

}
