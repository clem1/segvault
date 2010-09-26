/*
bf3/oracle.c
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
bf3/oracle.c
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

/* [_Jeremy Brown_ 7th Generation Fuzzing Oracle _Jeremy Brown_] 95 */

char of1[550], of2[1100], of3[2100], of4[4200], of5[8400],
     of6[16500], of7[33000], of8[65800], of9[131200], of10[262400],
     of11[525000], of12[1050000];

char fmt1[] = "%n%n%n%n%n", fmt2[] = "%p%p%p%p%p", fmt3[] = "%s%s%s%s%s",
     fmt4[] = "%d%d%d%d%d", fmt5[] = "%x%x%x%x%x", fmt6[] = "%s%p%x%d",
     fmt7[] = "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
     fmt8[] = "%.1024d", fmt9[] = "%.1025d", fmt10[] = "%.2048d",
     fmt11[] = "%.2049d", fmt12[] = "%.4096d", fmt13[] = "%.4097d",
     fmt14[] = "%99999999999s", fmt15[] = "%0%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18%19%20", fmt16[] = "%%20n",
     fmt17[] = "%%20p", fmt18[] = "%%20s", fmt19[] = "%%20d",
     fmt20[] = "%%20x", fmt21[] = "%#0123456x%08x%x%s%p%d%n%o%u%c%h%l%q%j%z%Z%t%i%e%g%f%a%C%S%08x%%";

char num1[] = "0", num2[] = "-0", num3[] = "1",
     num4[] = "-1", num5[] = "32767", num6[] = "-32768",
     num7[] = "65535", num8[] = "65536", num9[] = "65537",
     num10[] = "16777215", num11[] = "16777216", num12[] = "16777217",
     num13[] = "2147483647", num14[] = "-2147483647", num15[] = "2147483648",
     num16[] = "-2147483648", num17[] = "4294967294", num18[] = "4294967295",
     num19[] = "4294967296", num20[] = "357913942", num21[] = "-357913942",
     num22[] = "536870912", num23[] = "-536870912", num24[] = "5e-324",
     num25[] = "1.79769313486231E+308", num26[] = "3.39519326559384E-313", num27[] = "0xff",
     num28[] = "0x3fffffff", num29[] = "0xffffffff", num30[] = "0xfffffffe",
     num31[] = "0x3fffffff", num32[] = "0x7fffffff", num33[] = "0x7ffffffe",
     num34[] = "0x100", num35[] = "0x1000", num36[] = "0x10000",
     num37[] = "0x100000", num38[] = "0x80000000", num39[] = "-268435455",
     num40[] = "0x99999999", num41[] = "99999999999", num42[] = "-99999999999";

char misc1[] = "test|touch /tmp/FU_ZZ_ED|test", misc2[] = "test`touch /tmp/FU_ZZ_ED`test", misc3[] = "test'touch /tmp/FU_ZZ_ED'test",
     misc4[] = "test;touch /tmp/FU_ZZ_ED;test", misc5[] = "test&&touch /tmp/FU_ZZ_ED&&test", misc6[] = "test|C:/WINDOWS/system32/calc.exe|test",
     misc7[] = "test`C:/WINDOWS/system32/calc.exe`test", misc8[] = "test'C:/WINDOWS/system32/calc.exe'test", misc9[] = "test;C:/WINDOWS/system32/calc.exe;test",
     misc10[] = "C:/WINDOWS/system32/calc.exe", misc11[] = "|/bin/sh|", misc12[] = "`/bin/sh`",
     misc13[] = "%0xa", misc14[] = "%u000", misc15[] = "`~!@#$",
     misc16[] = "%^&*()", misc17[] = "-=_+", misc18[] = "[]\{}",
     misc19[] = "|;\\':", misc20[] = ",./<>?";

struct fzorc fuzz[] =
{

     // overflows
     {"", "Overflow: A x 550"},
     {"", "Overflow: A x 1100"},
     {"", "Overflow: A x 2100"},
     {"", "Overflow: A x 4200"},
     {"", "Overflow: A x 8400"},
     {"", "Overflow: A x 16500"},
     {"", "Overflow: A x 33000"},
     {"", "Overflow: A x 65800"},
     {"", "Overflow: A x 131200"},
     {"", "Overflow: A x 262400"},
     {"", "Overflow: A x 525000"},
     {"", "Overflow: A x 1050000"},

     // format strings
     {fmt1,  "Format String: %n x 5"},
     {fmt2,  "Format String: %p x 5"},
     {fmt3,  "Format String: %s x 5"},
     {fmt4,  "Format String: %d x 5"},
     {fmt5,  "Format String: %x x 5"},
     {fmt6,  "Format String: %s%p%x%d"},
     {fmt7,  "Format String: %s x 30"},
     {fmt8,  "Format String: %.1024d"},
     {fmt9,  "Format String: %.1025d"},
     {fmt10, "Format String: %.2048d"},
     {fmt11, "Format String: %.2049d"},
     {fmt12, "Format String: %.4096d"},
     {fmt13, "Format String: %.4097d"},
     {fmt14, "Format String: %99999999999s"},
     {fmt15, "Format String: %0%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18%19%20"},
     {fmt16, "Format String: %%20n"},
     {fmt17, "Format String: %%20p"},
     {fmt18, "Format String: %%20s"},
     {fmt19, "Format String: %%20d"},
     {fmt20, "Format String: %%20x"},
     {fmt21, "Format String: %#0123456x%08x%x%s%p%d%n%o%u%c%h%l%q%j%z%Z%t%i%e%g%f%a%C%S%08x%%"},

     // numbers
     {num1, "Number: 0"},
     {num2, "Number: -0"},
     {num3, "Number: 1"},
     {num4, "Number: -1"},
     {num5, "Number: 32767"},
     {num6, "Number: -32768"},
     {num7, "Number: 65535"},
     {num8, "Number: 65536"},
     {num9, "Number: 65537"},
     {num10, "Number: 16777215"},
     {num11, "Number: 16777216"},
     {num12, "Number: 16777217"},
     {num13, "Number: 2147483647"},
     {num14, "Number: -2147483647"},
     {num15, "Number: 2147483648"},
     {num16, "Number: -2147483648"},
     {num17, "Number: 4294967294"},
     {num18, "Number: 4294967295"},
     {num19, "Number: 4294967296"},
     {num20, "Number: 357913942"},
     {num21, "Number: -357913942"},
     {num22, "Number: 536870912"},
     {num23, "Number: -536870912"},
     {num24, "Number: 5e-324"},
     {num25, "Number: 1.79769313486231E+308"},
     {num26, "Number: 3.39519326559384E-313"},
     {num27, "Number: 0xff"},
     {num28, "Number: 0x3fffffff"},
     {num29, "Number: 0xffffffff"},
     {num30, "Number: 0xfffffffe"},
     {num31, "Number: 0x3fffffff"},
     {num32, "Number: 0x7fffffff"},
     {num33, "Number: 0x7ffffffe"},
     {num34, "Number: 0x100"},
     {num35, "Number: 0x1000"},
     {num36, "Number: 0x10000"},
     {num37, "Number: 0x100000"},
     {num38, "Number: 0x80000000"},
     {num39, "Number: -268435455"},
     {num40, "Number: 0x99999999"},
     {num41, "Number: 99999999999"},
     {num42, "Number: -99999999999"},

     // misc bugs
     {misc1,  "Misc Bug: test|touch /tmp/FU_ZZ_ED|test"},
     {misc2,  "Misc Bug: test`touch /tmp/FU_ZZ_ED`test"},
     {misc3,  "Misc Bug: test'touch /tmp/FU_ZZ_ED'test"},
     {misc4,  "Misc Bug: test;touch /tmp/FU_ZZ_ED;test"},
     {misc5,  "Misc Bug: test&&touch /tmp/FU_ZZ_ED&&test"},
     {misc6,  "Misc Bug: test|C:/WINDOWS/system32/calc.exe|test"},
     {misc7,  "Misc Bug: test`C:/WINDOWS/system32/calc.exe`test"},
     {misc8,  "Misc Bug: test'C:/WINDOWS/system32/calc.exe'test"},
     {misc9,  "Misc Bug: test;C:/WINDOWS/system32/calc.exe;test"},
     {misc10, "Misc Bug: C:/WINDOWS/system32/calc.exe"},
     {misc11, "Misc Bug: |/bin/sh|"},
     {misc12, "Misc Bug: `/bin/sh`"},
     {misc13, "Misc Bug: %0xa"},
     {misc14, "Misc Bug: %u000"},
     {misc15, "Misc Bug: `~!@#$"},
     {misc16, "Misc Bug: %^&*()"},
     {misc17, "Misc Bug: -=_+"},
     {misc18, "Misc Bug: []\{}"},
     {misc19, "Misc Bug: |;\\':"},
     {misc20, "Misc Bug: ,./<>?"},

};

/* [_Jeremy Brown_ 7th Generation Fuzzing Oracle _Jeremy Brown_] 95 */

void fzof_gen(void)
{

     memset(of1, 'A', sizeof(of1));
     fuzz[0].data = of1;
     memset(of2, 'A', sizeof(of2));
     fuzz[1].data = of2;
     memset(of3, 'A', sizeof(of3));
     fuzz[2].data = of3;
     memset(of4, 'A', sizeof(of4));
     fuzz[3].data = of4;
     memset(of5, 'A', sizeof(of5));
     fuzz[4].data = of5;
     memset(of6, 'A', sizeof(of6));
     fuzz[5].data = of6;
     memset(of7, 'A', sizeof(of7));
     fuzz[6].data = of7;
     memset(of8, 'A', sizeof(of8));
     fuzz[7].data = of8;
     memset(of9, 'A', sizeof(of9));
     fuzz[8].data = of9;
     memset(of10, 'A', sizeof(of10));
     fuzz[9].data = of10;
     memset(of11, 'A', sizeof(of11));
     fuzz[10].data = of11;
     memset(of12, 'A', sizeof(of12));
     fuzz[11].data = of12;

}
