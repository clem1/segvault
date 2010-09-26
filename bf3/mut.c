/*
bf3/mut.c
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
bf3/mut.c
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
#include "include/mod/html.h"
#include "include/mod/xml.h"
#include "include/mod/css.h"

void mutate(char *outp, char *mfn1, char *mfn2, char *fuzz, int x, int mod)
{

char fn[128], *modn = NULL;
int dsize = 0, pos = 0, fsz1 = 0, fsz2 = 0;
FILE *fd;

if(mod == 1) modn = "css";
if(mod == 2) modn = "dom";
if(mod == 3) modn = "html";
if(mod == 4) modn = "js";
if(mod == 5) modn = "xml";

if(mfn2 != NULL)
{
     
fd = fopen(mfn2, "r");

if(!fd)
{

     perror("fopen");
     exit(-1);

}

     fseek(fd, 0, SEEK_END);
fsz2 = ftell(fd);
     rewind(fd);

}

char rbuf2[fsz2+1];

if(mfn2 != NULL)
{
     memset(rbuf2, 0, sizeof(rbuf2));
     fread(rbuf2, 1, fsz2, fd);
     fclose(fd);

}

fd = fopen(mfn1, "r");

if(!fd)
{

     perror("fopen");
     exit(-1);

}

     fseek(fd, 0, SEEK_END);
fsz1 = ftell(fd);
     rewind(fd);

char rbuf1[fsz1+strlen(fuzz)+1], tmp[fsz1+strlen(fuzz)+1], end[fsz1+1];

     memset(rbuf1, 0, sizeof(rbuf1));
     fread(rbuf1, 1, fsz1, fd);
     fclose(fd);

for(pos = 0; pos <= fsz1; pos++)
{

     counter(&cnt);

     snprintf(fn, sizeof(fn), "%s/%s%d.html", outp, modn, cnt);

     memset(tmp, 0, sizeof(tmp));
     memset(end, 0, sizeof(end));

     strncpy(tmp, rbuf1, sizeof(tmp)-1);

     strncpy(end, tmp+pos, sizeof(end)-1);
     strncpy(tmp+pos, fuzz, sizeof(tmp)-pos-1);
     strncat(tmp, end, sizeof(tmp)-strlen(tmp)-1);

tmp[strlen(tmp)] = '\0';

char data[sizeof(tmp)+fsz2+512+1];

     memset(data, 0, sizeof(data));

if(mod == 1)
{

if(mfn2 == NULL) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s%s%d%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHC, CSSMA, modn, cnt, CSSMB, HTMLB);
if(mfn2 != NULL) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s%s%d%s\n%s\n\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHC, CSSMA, modn, cnt, CSSMB, HTMLB, rbuf2);

     cli_eng(data, fn);

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     snprintf(fn, sizeof(fn), "%s/%s%d.css", outp, modn, cnt);

     snprintf(data, sizeof(data), "%s", tmp);

}

if(mod == 2) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, HTMLB, tmp);

if(mod == 3) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, HTMLB, tmp);

if(mod == 4) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s\n\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, HTMLB, tmp);

if(mod == 5)
{

if(mfn2 == NULL) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s%s%d%s\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, XMLB, modn, cnt, XMLC, HTMLB);
if(mfn2 != NULL) snprintf(data, sizeof(data), "%s\n%s%s%d.html%s\n%s%s%s%s%d%s\n\n%s", HTMLA, REFRESHA, modn, cnt+1, REFRESHB, XMLMA, XMLMB, XMLMC, modn, cnt, XMLMD, rbuf2);

     cli_eng(data, fn);

     memset(data, 0, sizeof(data));
     memset(fn, 0, sizeof(fn));

     snprintf(fn, sizeof(fn), "%s/%s%d.xml", outp, modn, cnt);

     snprintf(data, sizeof(data), "%s", tmp);

}

     cli_eng(data, fn);

}
}
