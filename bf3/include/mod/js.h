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

#define JSTL sizeof(jsfunc)/sizeof(char *)

#define JSATL 3-1

#define JSO 5-1 // JS (3)
#define JSSA  2 // + 2 attended fuzzing options

#define JSA "<script type=\"text/javascript\">"
#define JSB "</script>"

#define JSDATA "var st = \"fuzz\"; var da = new Date(); var nu = new Number(2000); var xm = new Xml(\"<xml><coin><foo attr=\"foo\"></foo></coin></xml>\"); var bo = new Boolean(); var re = new RegExp(\"haha\", \"gi\"); var it = new Iterator(); var ex = new Exception(); var ge = new Generator(); var nm = new NameSpace(); var sc = new Script(); var ar = new Array(); var fu = new Function();"

#if 0
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
#endif

static char *jsfunc[] = { "st.quote", "st.toSource", "st.toString", "st.valueOf", "st.substring",
                          "st.toLowerCase", "st.toUpperCase", "st.charAt", "st.charCodeAt",
                          "st.indexOf", "st.lastIndexOf", "st.toLocaleLowerCase", "st.toLocaleUpperCase", 
                          "st.localeCompare", "st.match", "st.search", "st.replace", "st.split", "st.substr",
                          "st.concat", "st.slice", "st.bold", "st.italics", "st.fixed", "st.fontsize", 
                          "st.fontcolor", "st.link", "st.anchor", "st.strike", "st.small", "st.big", 
                          "st.blink", "st.sup", "st.sub", "String.fromCharCode",
                          "it.item", "it.bindMethod",
                          "bo.toSource", "bo.toString", "bo.valueOf",
                          "it.iterator", "it.next", "ge.iterator", "ge.next", "ge.send", "ge.throw",
                          "ge.close", "ex.toSource", "ex.toString", "na.toString", "qn.toString",
                          "xm.addNamespace", "xm.appendChild", "xm.attribute", "xm.attributes", 
                          "xm.child", "xm.childIndex", "xm.children", "xm.comments", "xm.contains", 
                          "xm.copy", "xm.descendants", "xm.elements", "xm.hasOwnProperty", "xm.hasComplexContent",
                          "xm.hasSimpleContent", "xm.inScopeNamespaces", "xm.insertChildAfter", "xm.insertChildBefore", 
                          "xm.length", "xm.localName", "xm.name", "xm.namespace", "xm.namespaceDeclarations", 
                          "xm.nodeKind", "xm.normalize", "xm.xml_parent", "xm.processingInstructions", "xm.prependChild",
                          "xm.propertyIsEnumerable", "xm.removeNamespace", "xm.replace", "xm.setChildren", "xm.setLocalName", 
                          "xm.setName", "xm.setNamespace", "xm.text", "xm.toString", "xm.toXMLString", "xm.toSource",
                          "xm.valueOf", "Xml.settings", "Xml.setSettings", "Xml.defaultSettings", "re.toSource", 
                          "re.toString", "re.compile", "re.exec", "re.test", "ob.toSource", "ob.toString", 
                          "ob.toLocaleString", "ob.valueOf", "ob.eval", "ob.watch", "ob.unwatch", "ob.hasOwnProperty", 
                          "ob.isPrototypeOf", "ob.propertyIsEnumerable", "ob.defineGetter", "ob.defineSetter", 
                          "ob.lookupGetter", "ob.lookupSetter", "sc.toSource", "sc.toString", "sc.compile", 
                          "sc.exec", "sc.freeze", "sc.thaw", "Script.thaw", "Math.toSource", "Math.abs", 
                          "Math.acos", "Math.asin", "Math.atan", "Math.atan2", "Math.ceil", "Math.cos", 
                          "Math.exp", "Math.floor", "Math.log", "Math.max", "Math.min", "Math.pow", 
                          "Math.random", "Math.round", "Math.sin", "Math.sqrt", "Math.tan", "fu.toSource", 
                          "fu.toString", "fu.apply", "fu.__applyConstructor__", "ar.toSource", "ar.toString", 
                          "ar.toLocaleString", "ar.join", "ar.reverse", "ar.sort", "ar.push", "ar.pop", "ar.shift", 
                          "ar.unshift", "ar.splice", "ar.concat", "ar.slice", "ar.indexOf", "ar.lastIndexOf", "ar.forEach",
                          "ar.map", "ar.filter", "ar.some", "ar.every", "Date.UTC", "Date.parse", "Date.now", "da.getTime", 
                          "da.getTimezoneOffset", "da.getYear", "da.getFullYear", "da.getUTCFullYear", "da.getMonth",
                          "da.getUTCMonth", "da.getDate", "da.getUTCDate", "da.getDay", "da.getUTCDay", "da.getHours", 
                          "da.getUTCHours", "da.getMinutes", "da.getUTCMinutes", "da.getSeconds", "da.getUTCSeconds", 
                          "da.getMilliseconds", "da.getUTCMilliseconds", "da.setTime", "da.setYear", "da.setFullYear", 
                          "da.setUTCFullYear", "da.setMonth", "da.setUTCMonth", "da.setDate", "da.setUTCDate", "da.setHours",
                          "da.setUTCHours", "da.setMinutes", "da.setUTCMinutes", "da.setSeconds", "da.setUTCSeconds", 
                          "da.setMilliseconds", "da.setUTCMilliseconds", "da.toUTCString", "da.toLocaleString", "da.toLocaleDateString",
                          "da.toLocaleTimeString", "da.toLocaleFormat", "da.toDateString", "da.toTimeString", "da.toSource",
                          "da.toString", "da.valueOf", "nu.toSource", "nu.toString", "nu.toLocaleString", "nu.valueOf",
                          "nu.toFixed", "nu.toExponential", "nu.toPrecision" };

static char *jsfunca[] = {"alert", "confirm", "prompt"};

////////////////////////////////////////////////////////////////////////////////////////////////////
