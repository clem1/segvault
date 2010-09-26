/*
bf3/include/mod/xml.h
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
bf3/include/mod/xml.h
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

/* jbrown@krakowlabs.com 12.27.2009

XML Module

Add more goodies if you like..

<!fuzz root (ANY)>

<!ELEMENT fuzz (ANY)>
<!ELEMENT test (fuzz)>
<!ELEMENT test (fuzz, fuzz)>
<!ELEMENT test (#fuzz)>

<?xml version="1.0"?>
<!DOCTYPE root [ <!ELEMENT test (fuzz)> ]>
<root>
<test>test</test> (**** test = fuzz ****)
</root>

*/

#define XMLTL 6-1

#define XMLO 16-1 // XML (14)

#define XMLA "<?xml version = \"1.0\"?>"
#define XMLB "<iframe src = \""
#define XMLC ".xml\">"

#define XMLMA "<script type=\"text/javascript\">\nvar Document=null;\n"
#define XMLMB "if(window.XMLHttpRequest) { xmlhttp = new XMLHttpRequest(); }\n"
#define XMLMC "xmlhttp.open(\"GET\", \""
#define XMLMD ".xml\", false);\nxmlhttp.send(\"\");\nDocument = xmlhttp.responseXML;"

#define XMLDATAA "<"
#define XMLDATAB ">"
#define XMLDATAC "</"
#define XMLDATAD "<root>"
#define XMLDATAE "</root>"
#define XMLDATAF "<element"
#define XMLDATAG "<element test = \""
#define XMLDATAH "<element test = \"123\">"
#define XMLDATAI "</element>"
#define XMLDATAJ "<child"
#define XMLDATAK "<child test = \""
#define XMLDATAL "<child test = \"123\">"
#define XMLDATAM "</child>"
#define XMLDATAN "\">"
#define XMLDATAO "= \"123\">"
#define XMLDATAP "<?xml version = \"1.0\""
#define XMLDATAQ "= \"123\"?>"
#define XMLDATAR "<?xml version = \""
#define XMLDATAS "<?xml version = \"1.0\" encoding = \""
#define XMLDATAT "\"?>"
#define XMLDATAU "fuzz"
#define XMLDATAV "<?xml-stylesheet href = \"fuzz.css\""
#define XMLDATAW "<?xml-stylesheet type = \"text/css\""
#define XMLDATAX "<?xml-stylesheet href = \"fuzz.css\" type = \"text/css\""
#define XMLDATAY "= \""

static char *xmlssa[] = {"href", "type", "title", "media", "charset", "alternate"};

////////////////////////////////////////////////////////////////////////////////////////////////////
