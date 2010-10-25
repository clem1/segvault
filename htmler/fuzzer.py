#    Fuzzer
#    Copyright (C) 2007  Petko D. Petkov (GNUCITIZEN)
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

__version__ = '1.5'
__author__ = 'Petko D. Petkov; pdp (architect)'

__doc__ = """
Fuzzer (GNUCITIZEN) http://www.gnucitizen.org
by Petko D. Petkov; pdp (architect)
for Python 2.5

browser fuzzing simplified
"""

#
# GLOBAL IMPORTS
#
import re
import logging

#
# DICTIONARIES
#
dict_string_values = [
    '.',
    '..',
    '.' * 1000000,
    '%00',
    '%00' * 1000000,
    '\x00',
    '\x00' * 1000000,
    '\x0A',
    '\x0A' * 1000000,
    '\x0D',
    '\x0D' * 1000000,
    '\x0D\x0A',
    '\x0D\x0A' * 1000000,
    '&#x00;',
    '&#x00;' * 1000000,
    '&#x0A;',
    '&#x0A;' * 1000000,
    '&#x0D;',
    '&#x0D;' * 1000000,
    '&#x0D;&#x0A;',
    '&#x0D;&#x0A;' * 1000000,
    '../' * 1000000,
    '..\\' * 1000000,
    "'",
    "'" * 1000000,
    '"',
    '"' * 1000000,
    '>',
    '<',
    '<>',
    '<' + 'A' * 1000000 + '>',
    '&' + 'A' * 1000000 + ';'
    'A' * 1000000,
    "'" + 'A' * 1000000 + "'",
    'A' * 1000000,
    "%x%x%x%x%x%n%n%n%n%n",
]

dict_number_values = [
    0,
    -1,
    0x100,
    0x1000,
    0x10000,
    0x100000,
    0xffffffff,
    0xfffffffe,
    0x80000000,
    0x7fffffff,
    0x7ffffffe,
    0x3fffffff
]

dict_rels = [
    'stylesheet'
]

dict_media = [
    'screen',
    'print'
]

dict_attributes = [
    'abbr',
    'accept',
    'accept-charset',
    'accesskey',
    'action',
    'align',
    'alink',
    'alt',
    'archive',
    'axis',
    'background',
    'bgcolor',
    'border',
    'cellpadding',
    'cellspacing',
    'char',
    'charoff',
    'charset',
    'checked',
    'cite',
    'class',
    'classid',
    'clear',
    'code',
    'codebase',
    'codetype',
    'color',
    'cols',
    'colspan',
    'compact',
    'content',
    'coords',
    'data',
    'datetime',
    'declare',
    'defer',
    'dir',
    'disabled',
    'enctype',
    'face',
    'for',
    'frame',
    'frameborder',
    'headers',
    'height',
    'href',
    'hreflang',
    'hspace',
    'http-equiv',
    'id',
    'ismap',
    'label',
    'lang',
    'language',
    'link',
    'longdesc',
    'marginheight',
    'marginwidth',
    'maxlength',
    'media',
    'method',
    'multiple',
    'name',
    'nohref',
    'noresize',
    'noshade',
    'nowrap',
    'object',
    'onblur',
    'onchange',
    'onclick',
    'ondblclick',
    'onfocus',
    'onkeydown',
    'onkeypress',
    'onkeyup',
    'onload',
    'onmousedown',
    'onmousemove',
    'onmouseout',
    'onmouseover',
    'onmouseup',
    'onreset',
    'onselect',
    'onsubmit',
    'onunload',
    'profile',
    'prompt',
    'readonly',
    'rel',
    'rev',
    'rows',
    'rowspan',
    'rules',
    'scheme',
    'code',
    'text',
    'onreset',
    'cols',
    'datetime',
    'disabled',
    'accept-charset',
    'shape',
    'usemap',
    'alt',
    'compact',
    'onload',
    'style',
    'title',
    'valuetype',
    'onmousemove',
    'valign',
    'accesskey',
    'onsubmit',
    'onkeypress',
    'rules',
    'nohref',
    'onmouseover',
    'background',
    'scrolling',
    'name',
    'bgcolor',
    'summary',
    'noshade',
    'coords',
    'onkeyup',
    'dir',
    'frame',
    'codetype',
    'ismap',
    'onchange',
    'hspace',
    'nowrap',
    'for',
    'selected',
    'rev',
    'label',
    'content',
    'onselect',
    'rel',
    'onfocus',
    'charoff',
    'method',
    'alink',
    'onkeydown',
    'codebase',
    'span',
    'src',
    'language',
    'standby',
    'longdesc',
    'maxlength',
    'action',
    'tabindex',
    'color',
    'colspan',
    'xml:space',
    'height',
    'href',
    'vlink',
    'size',
    'rows',
    'checked',
    'start',
    'width',
    'onmouseup',
    'scope',
    'scheme',
    'type',
    'cite',
    'onblur',
    'onmouseout',
    'link',
    'hreflang',
    'onunload',
    'target',
    'align',
    'value',
    'headers',
    'vspace',
    'declare',
    'classid',
    'defer',
    'prompt',
    'accept',
    'onmousedown',
    'char',
    'border',
    'id',
    'axis',
    'rowspan',
    'media',
    'charset',
    'archive',
    'readonly',
    'onclick',
    'cellspacing',
    'profile',
    'multiple',
    'object',
    'cellpadding',
    'http-equiv',
    'marginheight',
    'data',
    'class',
    'frameborder',
    'enctype',
    'lang',
    'xmlns',
    'clear',
    'face',
    'xml:lang',
    'marginwidth',
    'ondblclick',
    'abbr',
    'scope',
    'scrolling',
    'selected',
    'shape',
    'size',
    'span',
    'src',
    'standby',
    'start',
    'style',
    'summary',
    'tabindex',
    'target',
    'text',
    'title',
    'type',
    'usemap',
    'valign',
    'value',
    'valuetype',
    'version',
    'vlink',
    'vspace',
    'width'
]

dict_tags = [
    'code',
    'kbd',
    'font',
    'noscript',
    'style',
    'img',
    'title',
    'menu',
    'tt',
    'tr',
    'param',
    'li',
    'tfoot',
    'th',
    'input',
    'td',
    'dl',
    'blockquote',
    'fieldset',
    'big',
    'dd',
    'abbr',
    'optgroup',
    'dt',
    'button',
    'isindex',
    'p',
    'small',
    'div',
    'dir',
    'em',
    'meta',
    'sub',
    'bdo',
    'label',
    'basefont',
    'sup',
    'body',
    'acronym',
    'base',
    'br',
    'address',
    'strong',
    'legend',
    'ol',
    'script',
    'caption',
    's',
    'col',
    'h2',
    'h3',
    'h1',
    'h6',
    'h4',
    'h5',
    'table',
    'select',
    'noframes',
    'span',
    'area',
    'dfn',
    'var',
    'cite',
    'thead',
    'head',
    'option',
    'form',
    'hr',
    'strike',
    'link',
    'b',
    'colgroup',
    'ul',
    'applet',
    'del',
    'iframe',
    'pre',
    'ins',
    'tbody',
    'html',
    'samp',
    'map',
    'object',
    'a',
    'center',
    'textarea',
    'i',
    'q',
    'u',
    'a',
    'abbr',
    'acronym',
    'address',
    'applet',
    'area',
    'b',
    'base',
    'basefont',
    'bdo',
    'big',
    'blockquote',
    'body',
    'br',
    'button',
    'caption',
    'center',
    'cite',
    'code',
    'col',
    'colgroup',
    'dd',
    'del',
    'dfn',
    'dir',
    'div',
    'dl',
    'dt',
    'em',
    'fieldset',
    'font',
    'form',
    'frame',
    'frameset',
    'h1',
    'h2',
    'h3',
    'h4',
    'h5',
    'h6',
    'head',
    'hr',
    'html',
    'i',
    'iframe',
    'img',
    'input',
    'ins',
    'isindex',
    'kbd',
    'label',
    'legend',
    'li',
    'link',
    'map',
    'menu',
    'meta',
    'noframes',
    'noscript',
    'object',
    'ol',
    'optgroup',
    'option',
    'p',
    'param',
    'pre',
    'q',
    's',
    'samp',
    'script',
    'select',
    'small',
    'span',
    'strike',
    'strong',
    'style',
    'sub',
    'sup',
    'table',
    'tbody',
    'td',
    'textarea',
    'tfoot',
    'th',
    'thead',
    'title',
    'tr',
    'tt',
    'u',
    'ul',
    'var'
]

dict_css_properties = [
    'azimuth',
    'background',
    'background-attachment',
    'background-color',
    'background-image',
    'background-position',
    'background-repeat',
    'border',
    'border-bottom',
    'border-bottom-color',
    'border-bottom-style',
    'border-bottom-width',
    'border-collapse',
    'border-color',
    'border-left',
    'border-left-color',
    'border-left-style',
    'border-left-width',
    'border-right',
    'border-right-color',
    'border-right-style',
    'border-right-width',
    'border-spacing',
    'border-style',
    'border-top',
    'border-top-color',
    'border-top-style',
    'border-top-width',
    'border-width',
    'bottom',
    'caption-side',
    'clear',
    'clip',
    'color',
    'content',
    'counter-increment',
    'counter-reset',
    'cue',
    'cue-after',
    'cue-before',
    'cursor',
    'direction',
    'display',
    'elevation',
    'empty-cells',
    'float',
    'font',
    'font-family',
    'font-size',
    'font-style',
    'font-variant',
    'font-weight',
    'height',
    'left',
    'letter-spacing',
    'line-height',
    'list-style',
    'list-style-image',
    'list-style-position',
    'list-style-type',
    'margin',
    'margin-bottom',
    'margin-left',
    'margin-right',
    'margin-top',
    'max-height',
    'max-width',
    'min-height',
    'min-width',
    'orphans',
    'outline',
    'outline-color',
    'outline-style',
    'outline-width',
    'overflow',
    'padding',
    'padding-bottom',
    'padding-left',
    'padding-right',
    'padding-top',
    'page-break-after',
    'page-break-before',
    'page-break-inside',
    'pause',
    'pause-after',
    'pause-before',
    'pitch',
    'pitch-range',
    'play-during',
    'position',
    'quotes',
    'richness',
    'right',
    'speak',
    'speak-header',
    'speak-numeral',
    'speak-punctuation',
    'speech-rate',
    'stress',
    'table-layout',
    'text-align',
    'text-decoration',
    'text-indent',
    'text-transform',
    'top',
    'unicode-bidi',
    'vertical-align',
    'visibility',
    'voice-family',
    'volume',
    'white-space',
    'widows',
    'width',
    'word-spacing',
    'z-index'
]

dict_css_values = [
    '100',
    '200',
    '300',
    '400',
    '500',
    '600',
    '700',
    '800',
    '900',
    'above',
    'absolute',
    'always',
    'armenian',
    'attr(<placeholder>)',
    'auto',
    'avoid',
    'background-color',
    'baseline',
    'behind',
    'below',
    'bidi-override',
    'blink',
    'block',
    'bold',
    'bolder',
    'border-top-color',
    'both',
    'bottom',
    'capitalize',
    'caption',
    'center',
    'center-left',
    'center-right',
    'circle',
    'close-quote',
    'code',
    'collapse',
    'continuous',
    'crosshair',
    'decimal',
    'decimal-leading-zero',
    'default',
    'digits',
    'disc',
    'e-resize',
    'embed',
    'far-left',
    'far-right',
    'fast',
    'faster',
    'fixed',
    'georgian',
    'help',
    'hidden',
    'hide',
    'high',
    'higher',
    'icon',
    'inherit',
    'inline',
    'inline-block',
    'inline-table',
    'inside',
    'invert',
    'italic',
    'justify',
    'left',
    'left-side',
    'leftwards',
    'level',
    'lighter',
    'line-through',
    'list-item',
    'loud',
    'low',
    'lower',
    'lower-alpha',
    'lower-greek',
    'lower-latin',
    'lower-roman',
    'lowercase',
    'ltr',
    'medium',
    'menu',
    'message-box',
    'middle',
    'mix',
    'move',
    'n-resize',
    'ne-resize',
    'no-close-quote',
    'no-open-quote',
    'no-repeat',
    'none',
    'normal',
    'nowrap',
    'nw-resize',
    'oblique',
    'once',
    'open-quote',
    'outside',
    'overline',
    'pointer',
    'pre',
    'pre-line',
    'pre-wrap',
    'progress',
    'relative',
    'repeat',
    'repeat-x',
    'repeat-y',
    'right',
    'right-side',
    'rightwards',
    'rtl',
    'run-in',
    's-resize',
    'scroll',
    'se-resize',
    'separate',
    'show',
    'silent',
    'slow',
    'slower',
    'small-caps',
    'small-caption',
    'soft',
    'spell-out',
    'square',
    'static',
    'status-bar',
    'sub',
    'super',
    'sw-resize',
    'table',
    'table-caption',
    'table-cell',
    'table-column',
    'table-column-group',
    'table-footer-group',
    'table-header-group',
    'table-row',
    'table-row-group',
    'text',
    'text-bottom',
    'text-top',
    'top',
    'transparent',
    'underline',
    'upper-alpha',
    'upper-latin',
    'upper-roman',
    'uppercase',
    'url(<placeholder>)',
    'visible',
    'w-resize',
    'wait',
    'x-fast',
    'x-high',
    'x-loud',
    'x-low',
    'x-slow',
    'x-soft'
]

dict_meta_characters = [
    '~',
    '`',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '-',
    '_',
    '+',
    '=',
    '{',
    '}',
    '[',
    ']',
    '\\',
    '|',
    ';',
    ':',
    "'",
    '"',
    '<',
    '>',
    ',',
    '.',
    '/',
    '?'
]

dict_special_strings = [
    '..',
    '../',
    '..\\',
    '%00',
    '%0A',
    '%0D',
    '%0D%0A',
    '&#x00;',
    '&#x0A;',
    '&#x0D;',
    '&#x0D;&#x0A;'
]

dict_plain_strings = [
    'A' * 100,
    'A' * 1000,
    'A' * 10000
]

dict_protocols = [
    'mailto',
    'news',
    'nntp',
    'snews',
    'afp',
    'data',
    'disk',
    'hcp',
    'javascript',
    'mailto',
    'ms-help',
    'shell',
    'vbscript',
    'itms'
]

dict_protocol_delimiters = [
    ':',
    ':/',
    '://',
    ':///'
]

dict_mimes = [
    'application/asx',
    'application/divxplayer-plugi',
    'application/futuresplas',
    'application/itunes-plugi',
    'application/ogg',
    'application/pdf',
    'application/sdp',
    'application/vnd.adobe.x-mars',
    'application/vnd.adobe.xdp+xml',
    'application/vnd.adobe.xfd+xm',
    'application/vnd.adobe.xfdf',
    'application/vnd.fdf',
    'application/x-divxcontentuploa',
    'application/x-dr',
    'application/x-drm-v',
    'application/x-google-vlc-plugi',
    'application/x-mpeg',
    'application/x-mplayer2',
    'application/x-ogg',
    'application/x-rtsp',
    'application/x-sdp',
    'application/x-shockwave-flash',
    'application/x-silverligh',
    'application/x-vlc-plugin',
    'audio/3gpp',
    'audio/3gpp2',
    'audio/AMR',
    'audio/aac',
    'audio/aiff',
    'audio/basic',
    'audio/mid',
    'audio/midi',
    'audio/mp4',
    'audio/mpeg',
    'audio/vnd.qcel',
    'audio/wa',
    'audio/x-aac',
    'audio/x-aiff',
    'audio/x-caf',
    'audio/x-gsm',
    'audio/x-m4',
    'audio/x-m4a',
    'audio/x-m4p',
    'audio/x-midi',
    'audio/x-mpeg',
    'audio/x-ms-wax',
    'audio/x-ms-wma',
    'audio/x-wav',
    'image/pict',
    'image/png',
    'image/x-macpaint',
    'image/x-pict',
    'image/x-png',
    'image/x-quicktime',
    'image/x-sgi',
    'image/x-targ',
    'video/3gp',
    'video/3gpp2',
    'video/div',
    'video/flc',
    'video/mp4',
    'video/mpeg',
    'video/quicktime',
    'video/sd-video',
    'video/x-m4v',
    'video/x-mpe',
    'video/x-ms-asf',
    'video/x-ms-asf-plugin',
    'video/x-ms-wm',
    'video/x-ms-wmv',
    'video/x-ms-wv'
]

#
# GENERATORS
#
def generator_ff_javascript():
    """ discover arbitrary Sandboxes in Firefox """

    global ITERATION

    for tag in set(dict_tags):
        for attribute in set(dict_attributes):
            logging.info('''<%s %s="javascript:dump(%d + '--' + this + '\\n')"></%s>''' % (tag, attribute, ITERATION, tag))
            yield '''<%s %s="javascript:dump(%d + '--' + this + '\\n')"></%s>''' % (tag, attribute, ITERATION, tag)

def generator_ff_attribute_string():
    """ discover bugs in attribute """
    for tag in set(dict_tags):
        for attribute in set(dict_attributes):
            for string in dict_string_values:
                logging.info('''<%s %s="%s"/>''' % (tag, attribute, string))
                yield '''<%s %s="%s"/>''' % (tag, attribute, string)

#
# MAIN
#
if __name__ == '__main__':
    import sys
    import socket
    import thread
    import getopt
    import BaseHTTPServer

    def usage():
        print 'usage: %s [options] <generator>' % sys.argv[0]
        print '\t-b --bind        bind to IP address (default 127.0.0.1)'
        print '\t-p --port        listening port for the HTTP server (default 8000)'
        print '\t-s --skip        how many steps to skip (default 0)'
        print '\t-f --filter      filter out matching pattern (default ^$)'
        print '\t-d --dump        dump iteration'
        print '\t-t --template    output template (use %s as a placeholder)'
        print '\t-v --verbose     verbose mode'
        print '\t-l --list        list available generators'
        print '\t-i --import      import external python module'
        print '\t-h --help        show this screen'
        print
        print 'GNUCITIZEN'
        print 'Petko D. Petkov; pdp (architect)'

        sys.exit()

    HOST = '127.0.0.1'
    PORT = 8000
    SKIP = 0
    DUMP = 0
    FILTER = '^$'
    TEMPLATE = '<html><body onload="location.reload()">%s</body></html>'
    MODULES = []
    VERBOSE = 50
    ITERATION = 1
    GENERATOR = None

    try:
        opts, args = getopt.gnu_getopt(sys.argv[1:], 'b:p:s:d:f:t:i:vlh', ['bind=', 'port=', 'skip=', 'dump', 'filter=', 'template=', 'import=', 'verbose', 'list','help'])

        _i = 0
        _opts = opts

        for opt, val in opts:
            if opt == '-l' or opt == '--list':
                opts.append(opts.pop(_i))

            _i += 1

        for opt, val in opts:
            if opt == '-b' or opt == '--bind':
                HOST = val
                
            if opt == '-p' or opt == '--port':
                try:
                    PORT = int(val)

                except:
                    raise Exception('port value not integer')

            if opt == '-s' or opt == '--skip':
                try:
                    SKIP = int(val)

                except:
                    raise Exception('skip value not integer')

                if SKIP <= 0:
                    raise Exception('skip value must be greater then 0')

            if opt == '-d' or opt == '--dump':
                try:
                    DUMP = int(val)

                except:
                    raise Exception('dump value not integer')

                if DUMP <= 0:
                    raise Exception('dump value must be greater then 0')

            if opt == '-f' or opt == '--filter':
                FILTER = val

            if opt == '-t' or opt == '--template':
                TEMPLATE = val

            if opt == '-i' or opt == '--import':
                tokens = val.split('.')

                if len(tokens) == 1:
                    globals()[val] = __import__(val)
                    MODULES.append(val);
                    
                else:
                    globals()[tokens[-1:]] = __import__('.'.join(tokens[:-1]), fromlist=[tokens[-1:]])
                    MODULES.append(tokens[-1:])

            if opt == '-v' or opt == '--verbose':
                VERBOSE -= 10

            if opt == '-l' or opt == '--list':
                for name in MODULES:
                    module = globals()[name]

                    for key in dir(module):
                        if key.startswith('generator_') and callable(getattr(module, key)):
                            print key[10:], '-', str(getattr(module, key).__doc__).strip()

                globs = globals()
                
                for key in globs.keys():
                    if key.startswith('generator_') and callable(globs[key]):
                        print key[10:], '-', str(globs[key].__doc__).strip()

                sys.exit()

            if opt == '-h' or opt == '--help':
                usage()

        if len(args) == 0:
            raise Exception('no generator specified')

        try:
            for name in MODULES:
                module = globals()[name]

                try:
                    GENERATOR = getattr(module, 'generator_' + ' '.join(args).strip())

                except: pass

            if GENERATOR == None:
                GENERATOR = globals()['generator_' + ' '.join(args).strip()]

        except:
            raise Exception('generator not found')

    except Exception, e:
        print e
        print

        usage()

    logging.basicConfig(level=VERBOSE, format='-- %(levelname)s - %(message)s')
    logging.addLevelName(60, 'ITERATION')

    GENERATOR = GENERATOR()

    if DUMP > 0:
        for i in range(0, DUMP - 1):
            GENERATOR.next()
            ITERATION += 1

        value = GENERATOR.next()
        
        logging.log(60, ITERATION)
        print TEMPLATE % value

        sys.exit()
                    
    class YieldingBaseHTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
        def do_GET(self):
            global SKIP
            global FILTER
            global TEMPLATE
            global ITERATION
            global GENERATOR

            self.send_response(200)
            self.send_header('Content-Type', 'text/html')
            self.send_header('Cache-Control', 'no-store, no-cache, must-revalidate')
            self.send_header('Pragma', 'no-cache')
            self.end_headers()

            if self.path == '/fuzzing':
                try:
                    if SKIP != 0:
                       for i in range(0, SKIP):
                           GENERATOR.next()
                           ITERATION += 1

                       SKIP = 0

                    value = GENERATOR.next()
 
                    while re.search(FILTER, value):
                        value = GENERATOR.next()
                        ITERATION += 1

                    logging.log(60, ITERATION)
                    self.wfile.write(TEMPLATE % value)

                    ITERATION += 1
                except StopIteration:
                    self.wfile.write('DONE!')
                    server.stop()

    class StoppableHTTPServer(BaseHTTPServer.HTTPServer):
        def server_bind(self):
            BaseHTTPServer.HTTPServer.server_bind(self)

            self.socket.settimeout(1)
            self.run = True

        def get_request(self):
            while self.run:
                try:
                    sock, addr = self.socket.accept()
                    sock.settimeout(None)

                    return (sock, addr)

                except socket.timeout:
                    pass

        def stop(self):
            self.run = False

        def serve(self):
            while self.run:
                self.handle_request()

    server = StoppableHTTPServer((HOST, PORT), YieldingBaseHTTPHandler);
    thread.start_new_thread(server.serve, ())

    while server.run:
        pass
