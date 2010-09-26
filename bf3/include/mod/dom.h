/*
bf3/include/mod/dom.h
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
bf3/include/mod/dom.h
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

////////////////////////////////////////////////////////////////////////////////////////////////////

/* jbrown@krakowlabs.com 10.02.2009

(HTML/JS) DOM Module

*/

#define DOM1TL 12-1
#define DOM2TL  6-1
#define DOM3TL  6-1

#define DOMATL 14-1

#define DOMO 10-1 // DOM (8)
#define DOMA    2 // + 2 attended fuzzing options

#define DOMDATAA "document"
#define DOMDATAB "write"
#define DOMDATAC "location"
#define DOMDATAD "host"

// x.x(fuzz); & x.x("fuzz");
static char *domobj1[]  = {"document.getElementById", "document.getElementsByName", "document.getElementsByTagName", "document.open",
                           "document.write", "document.writeIn", "window.outerHeight", "window.outerWidth", "window.status",
                           "window.resizeBy", "window.resizeTo"};

// x.x(fuzz, fuzz); & x.x("fuzz", "fuzz");
static char *domobj2[] = {"window.moveBy", "window.moveTo", "window.scrollBy", "window.scrollTo", "window.setInterval",
                          "window.setTimeout"};

// x.x = fuzz; & x.x = "fuzz";
static char *domobje[] = {"document.cookie", "screen.bufferDepth", "screen.updateInterval", "window.defaultStatus", "location.hash",
                          "location.replace"};

// attended fuzzing
static char *domobja[] = {"history.go", "location.assign", "location.hostname", "location.href", "location.pathname",
                          "location.protocol", "location.search", "location.replace", "location.port", "history.go",
                          "window.alert", "window.confirm", "window.open", "window.prompt"};

////////////////////////////////////////////////////////////////////////////////////////////////////

// meta refresh goes in attended fuzzing
