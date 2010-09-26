/*
bf3/include/mod/html.h
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
bf3/include/mod/html.h
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

/* jbrown@krakowlabs.com 09.28.2009

HTML Module

*/

#define HTMLTL 48-1

#define HTMLO 6-1 // HTML (4)

#define HTMLA "<html>"
#define HTMLB "</html>"

#define HTMLDATAA "body"
#define HTMLDATAB "id"

static char *htmltag[]  = {"body", "p", "hr", "font", "bdo", "pre", "a", "link", "frame", "frameset",
                           "iframe", "form", "input", "textarea", "button", "select", "optgroup", "option", "label", "legend",
                           "ul", "ol", "li", "dir", "menu", "img", "map", "area", "table", "caption",
                           "th", "tr", "td", "thead", "tbody", "tfoot", "col", "colgroup", "style", "div",
                           "head", "meta", "base", "basefont", "script", "applet", "object", "param"};

static int htmltagn[]   = {14, 9, 12, 11, 9, 9, 18, 16, 16, 10, 18, 15, 20, 13, 12, 12, 10, 12, 9, 9,
                           10, 11, 10, 9, 9, 18, 8, 14, 17, 9, 22, 13, 22, 12, 12, 12, 14, 14, 10, 9,
                            9, 12, 9, 11, 13, 20, 24, 12};

static char *body[]     = {"alink", "background", "bgcolor", "link", "text", "vlink", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *p[]        = {"align", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *hr[]       = {"align", "noshade", "size", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *font[]     = {"color", "face", "size", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *bdo[]      = {"dir", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *pre[]      = {"width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *a[]        = {"charset", "coords", "href", "hreflang", "name", "rel", "rev", "shape", "target", "type", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *llink[]    = {"charset", "href", "hrefland", "media", "rel", "rev", "target", "type", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *frame[]    = {"frameborder", "longdesc", "marginheight", "marginwidth", "name", "noresize", "scrolling", "src", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *frameset[] = {"cols", "rows", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *iframe[]   = {"align", "frameborder", "height", "longdesc", "marginheight", "marginwidth", "name", "scrolling", "src", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *form[]     = {"action", "accept", "accept-charset", "enctype", "method", "name", "target", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *input[]    = {"accept", "align", "alt", "checked", "disabled", "maxlength", "name", "readonly", "size", "src", "type", "value", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *textarea[] = {"cols", "rows", "disabled", "name", "readonly", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *button[]   = {"disabled", "name", "type", "value", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *sselect[]  = {"disabled", "multiple", "name", "size", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *optgroup[] = {"label", "disabled", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *option[]   = {"disabled", "label", "selected", "value", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *label[]    = {"for", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *legend[]   = {"align", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *ul[]       = {"compact", "type", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *ol[]       = {"compact", "start", "type", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *li[]       = {"type", "value", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *dir[]      = {"compact", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *menu[]     = {"compact", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *img[]      = {"alt", "src", "align", "border", "height", "hspace", "longdesc", "usemap", "vspace", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *map[]      = {"id", "class", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *area[]     = {"alt", "coords", "href", "nohref", "shape", "target", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *table[]    = {"align", "bgcolor", "border", "cellpadding", "cellspacing", "frame", "rules", "summary", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *caption[]  = {"align", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *th[]       = {"abbr", "align", "axis", "bgcolor", "char", "charoff", "colspan", "headers", "height", "nowrap", "rowspan", "scope", "valign", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *tr[]       = {"align", "bgcolor", "char", "charoff", "valign", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *td[]       = {"abbr", "align", "axis", "bgcolor", "char", "charoff", "colspan", "headers", "height", "nowrap", "rowspan", "scope", "valign", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *thead[]    = {"align", "char", "charoff", "valign", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *tbody[]    = {"align", "char", "charoff", "valign", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *tfoot[]    = {"align", "char", "charoff", "valign", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *col[]      = {"align", "char", "charoff", "span", "valign", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *colgroup[] = {"align", "char", "charoff", "span", "valign", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *style[]    = {"type", "media", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *ddiv[]     = {"align", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *head[]     = {"profile", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *meta[]     = {"content", "http-equiv", "name", "scheme", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *base[]     = {"target", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *basefont[] = {"color", "face", "size", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *script[]   = {"type", "charset", "defer", "language", "src", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *applet[]   = {"height", "width", "align", "alt", "archive", "code", "codebase", "hspace", "name", "object", "title", "vspace", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *object[]   = {"align", "archive", "border", "classid", "codebase", "codetype", "data", "declare", "height", "hspace", "name", "standby", "type", "usemap", "vspace", "width", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};
static char *param[]    = {"name", "type", "value", "valuetype", "class", "id", "style", "title", "dir", "lang", "accesskey", "tabindex"};

////////////////////////////////////////////////////////////////////////////////////////////////////
