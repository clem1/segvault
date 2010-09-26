/*
bf3/include/mod/js.h
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
bf3/include/mod/js.h
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

/* jbrown@krakowlabs.com 09.30.2009

JavaScript Module

*/

#define JSTL 66-1

#define JSATL 3-1

#define JSO 5-1 // JS (3)
#define JSSA  2 // + 2 attended fuzzing options

#define JSA "<script type=\"text/javascript\">"
#define JSB "</script>"

#define JSDATA "var s = \"fuzz\"; var d = new Date(); var n = new Number(2000);"

static char *jsfunc[]  = {"decodeURI", "decodeURIComponent", "encodeURI", "encodeURIcomponent", "escape",
                          "unescape", "eval", "isFinite", "isNaN", "Number",
                          "parseFloat", "parseInt", "String", "s.anchor", "s.charAt",
                          "s.concat", "s.fontcolor", "s.fontsize", "s.fromCharCode", "s.indexOf",
                          "s.lastIndexOf", "s.link", "s.match", "s.replace", "s.search",
                          "s.slice", "s.split", "s.substr", "s.substring", "d.parse",
                          "d.setDate", "d.setFullYear", "d.setHours", "d.setMilliseconds", "d.setMinutes",
                          "d.setMonth", "d.Seconds", "d.setTime", "d.setUTCDate", "d.setUTCMonth",
                          "d.setUTCFullYear", "d.setUTCHours", "d.setUTCMinutes", "d.setURCSeconds", "d.setUTCMilliseconds",
                          "d.setYear", "Math.abs", "Math.acos", "Math.asin", "Math.atan",
                          "Math.atan2", "Math.ceil", "Math.cos", "Math.exp", "Math.floor",
                          "Math.log", "Math.max", "Math.min", "Math.pow", "Math.round",
                          "Math.sin", "Math.sqrt", "Math.tan", "n.toExponential", "n.toPercision",
                          "n.toString"};

static char *jsfunca[] = {"alert", "confirm", "prompt"};

////////////////////////////////////////////////////////////////////////////////////////////////////
