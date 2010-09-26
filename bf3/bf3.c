/*
bf3/bf3.c
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
bf3/bf3.c
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

void usage(char *prog)
{

     printf("%s", BANNER);
     printf("\nUSAGE:   %s <-M #> <-A/-U> <-O/-R #> [-Z #]", prog);
     printf("\n               [-X file.ext] [-x extra.html] [-P /www]\n");
     printf("\nINFO:    [-T Fuzzing Oracle] [-D Modules Available]\n\n");

     exit(0);

}

void list_modules(void)
{

     printf("%s", BANNER);
     printf("\n              [Modules Available]\n\n");
     printf("    (1) Cascading Style Sheets     (CSS) \n");
     printf("    (2) Document Object Model      (DOM)\n");
     printf("    (3) HyperText Markup Language  (HTML)\n");
     printf("    (4) JavaScript                 (JS) \n");
	printf("    (5) Extensible Markup Language (XML) \n\n");

     exit(0);

}

int main(int argc, char *argv[])
{

char *outp = NULL, opt, *mfn1 = NULL, *mfn2 = NULL;
int mod = 0, mode = 0, fzo = 0, rdn = 0, rsz = RNDSZ+1, mut = 0;

while ((opt = getopt(argc, argv, "M:AUOR:Z:X:x:P:TD")) != EOF)
{

     switch(opt) {

		       case 'M':
		       mod = atoi(optarg); // fuzzing module
		       break;

		       case 'A':
		       mode = 1; // attended fuzzing mode
		       break;

		       case 'U':
		       mode = 2; // unattended fuzzing mode
		       break;

		       case 'O':
		       fzo = 1; // use standard fuzzing oracle
		       break;

		       case 'R':
                       fzo = 2; // use random fuzzing generator
                       rdn = atoi(optarg);
		       break;

		       case 'Z':
                       rsz = atoi(optarg); // max random size of fuzzing data
		       break;

		       case 'X':
		       mut  = 1; // use mutation fuzzing based on file.ext
                 mfn1 = optarg;
		       break;

		       case 'x':
		       mut  = 1; // use mutation fuzzing based on file.ext with extra html
                 mfn2 = optarg;
		       break;

		       case 'P':
		       outp = optarg; // output directory
		       break;

		       case 'T':
		       list_oracle(); // list fuzzing oracle
		       break;

		       case 'D':
		       list_modules(); // list available modules
		       break;

		       default:
		       usage(argv[0]);
		       break;

		  }

}

if((mod < 1) && (mod > MODTL))              usage(argv[0]);
if((mode == 1) && (mod != 2) && (mod != 4)) usage(argv[0]); // dom and js modules support attended fuzzing
if((fzo < 1) || (fzo > 2))                  usage(argv[0]);
if((fzo == 2) && (rdn < 1))                 usage(argv[0]);
if((fzo == 2) && (rdn > RNDMAX))            rdn = RNDMAX;
if((fzo == 2) && (rsz < 1))                 usage(argv[0]);
if((fzo == 2) && (rsz > RNDMAX))            rsz = RNDMAX+1;
if((mfn2 != NULL) && (mfn1 == NULL))        usage(argv[0]);

     engine(outp, mod, mode, fzo, rdn, rsz, mfn1, mfn2, mut);

     return 0;

}
