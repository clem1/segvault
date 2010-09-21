# jumped in a river what did i see?
# black-eyed angels swimming with me

# SHAMELESS PORT OF LCAMTUF'S MANGELME
# nd@felinemenace.org - (c) 1984
# with a few extra hacks, finds elusive IE bugs ;)

import random

types = []
data = {}

data["A"] = ["NAME", "HREF", "REF", "REV", "TITLE", "TARGET", "SHAPE", "onLoad", "STYLE"]
data["APPLET"] = ["CODEBASE", "CODE", "NAME", "ALIGN", "ALT", "HEIGHT", "WIDTH", "HSPACE", "VSPACE", "DOWNLOAD", "HEIGHT", "NAME", "TITLE", "onLoad", "STYLE"]
data["AREA"] = ["SHAPE", "ALT", "CO-ORDS", "HREF", "onLoad", "STYLE"]
data["B"] = ["onLoad", "STYLE"]
data["BANNER"] = ["onLoad", "STYLE"]
data["BASE"] = ["HREF", "TARGET", "onLoad", "STYLE"]
data["BASEFONT"] = ["SIZE", "onLoad", "STYLE"]
data["BGSOUND"] = ["SRC", "LOOP", "onLoad", "STYLE"]
data["BQ"] = ["CLEAR", "NOWRAP", "onLoad", "STYLE"]
data["BODY"] = ["BACKGROUND", "BGCOLOR", "TEXT", "LINK", "ALINK", "VLINK", "LEFTMARGIN", "TOPMARGIN", "BGPROPERTIES", "onLoad", "STYLE"]
data["CAPTION"] = ["ALIGN", "VALIGN", "onLoad", "STYLE"]
data["CENTER"] = ["onLoad", "STYLE"]
data["COL"] = ["ALIGN", "SPAN", "onLoad", "STYLE"]
data["COLGROUP"] = ["ALIGN", "VALIGN", "HALIGN", "WIDTH", "SPAN", "onLoad", "STYLE"]
data["DIV"] = ["ALIGN", "CLASS", "LANG", "onLoad", "STYLE"]
data["EMBED"] = ["SRC", "HEIGHT", "WIDTH", "UNITS", "NAME", "PALETTE", "onLoad", "STYLE"]
data["FIG"] = ["SRC", "ALIGN", "HEIGHT", "WIDTH", "UNITS", "IMAGEMAP", "onLoad", "STYLE"]
data["FN"] = ["ID", "onLoad", "STYLE"]
data["FONT"] = ["SIZE", "COLOR", "FACE", "onLoad", "STYLE"]
data["FORM"] = ["ACTION", "METHOD", "ENCTYPE", "TARGET", "SCRIPT", "onLoad", "STYLE"]
data["FRAME"] = ["SRC", "NAME", "MARGINWIDTH", "MARGINHEIGHT", "SCROLLING", "FRAMESPACING", "onLoad", "STYLE"]
data["FRAMESET"] = ["ROWS", "COLS", "onLoad", "STYLE"]
data["H1"] = ["SRC", "DINGBAT", "onLoad", "STYLE"]
data["HEAD"] = ["onLoad", "STYLE"]
data["HR"] = ["SRC", "SIZE", "WIDTH", "ALIGN", "COLOR", "onLoad", "STYLE"]
data["HTML"] = ["onLoad", "STYLE"]
data["IFRAME"] = ["ALIGN", "FRAMEBORDER", "HEIGHT", "MARGINHEIGHT", "MARGINWIDTH", "NAME", "SCROLLING", "SRC", "ADDRESS", "WIDTH", "onLoad", "STYLE"]
data["IMG"] = ["ALIGN", "ALT", "SRC", "BORDER", "DYNSRC", "HEIGHT", "HSPACE", "ISMAP", "LOOP", "LOWSRC", "START", "UNITS", "USEMAP", "WIDTH", "VSPACE", "onLoad", "STYLE"]
data["INPUT"] = ["TYPE", "NAME", "VALUE", "onLoad", "STYLE"]
data["ISINDEX"] = ["HREF", "PROMPT", "onLoad", "STYLE"]
data["LI"] = ["SRC", "DINGBAT", "SKIP", "TYPE", "VALUE", "onLoad", "STYLE"]
data["LINK"] = ["REL", "REV", "HREF", "TITLE", "onLoad", "STYLE"]
data["MAP"] = ["NAME", "onLoad", "STYLE"]
data["MARQUEE"] = ["ALIGN", "BEHAVIOR", "BGCOLOR", "DIRECTION", "HEIGHT", "HSPACE", "LOOP", "SCROLLAMOUNT", "SCROLLDELAY", "WIDTH", "VSPACE", "onLoad", "STYLE"]
data["MENU"] = ["onLoad", "STYLE"]
data["META"] = ["HTTP-EQUIV", "CONTENT", "NAME", "onLoad", "STYLE"]
data["MULTICOL"] = ["COLS", "GUTTER", "WIDTH", "onLoad", "STYLE"]
data["NOFRAMES"] = ["onLoad", "STYLE"]
data["NOTE"] = ["CLASS", "SRC", "onLoad", "STYLE"]
data["OVERLAY"] = ["SRC", "X", "Y", "HEIGHT", "WIDTH", "UNITS", "IMAGEMAP", "onLoad", "STYLE"]
data["PARAM"] = ["NAME", "VALUE", "onLoad", "STYLE"]
data["RANGE"] = ["FROM", "UNTIL", "onLoad", "STYLE"]
data["SCRIPT"] = ["LANGUAGE", "onLoad", "STYLE"]
data["SELECT"] = ["NAME", "SIZE", "MULTIPLE", "WIDTH", "HEIGHT", "UNITS", "onLoad", "STYLE"]
data["OPTION"] = ["VALUE", "SHAPE", "onLoad", "STYLE"]
data["SPACER"] = ["TYPE", "SIZE", "WIDTH", "HEIGHT", "ALIGN", "onLoad", "STYLE"]
data["SPOT"] = ["ID", "onLoad", "STYLE"]
data["TAB"] = ["INDENT", "TO", "ALIGN", "DP", "onLoad", "STYLE"]
data["TABLE"] = ["ALIGN", "WIDTH", "BORDER", "CELLPADDING", "CELLSPACING", "BGCOLOR", "VALIGN", "COLSPEC", "UNITS", "DP", "onLoad", "STYLE"]
data["TBODY"] = ["CLASS", "ID", "onLoad", "STYLE"]
data["TD"] = ["COLSPAN", "ROWSPAN", "ALIGN", "VALIGN", "BGCOLOR", "onLoad", "STYLE"]
data["TEXTAREA"] = ["NAME", "COLS", "ROWS", "onLoad", "STYLE"]
data["TEXTFLOW"] = ["CLASS", "ID", "onLoad", "STYLE"]
data["TFOOT"] = ["COLSPAN", "ROWSPAN", "ALIGN", "VALIGN", "BGCOLOR", "onLoad", "STYLE"]
data["TH"] = ["ALIGN", "CLASS", "ID", "onLoad", "STYLE"]
data["TITLE"] = ["onLoad", "STYLE"]
data["TR"] = ["ALIGN", "VALIGN", "BGCOLOR", "CLASS", "onLoad", "STYLE"]
data["UL"] = ["SRC", "DINGBAT", "WRAP", "TYPE", "PLAIN", "onLoad", "STYLE"]

for x in data.keys():
 types.append(x)

ext = [".jar",".class",".jpg",".htm",".css",".gif",".png",".eml",".mpg",".wav",".mp3",".swf"]
pre = ["http:","https:","url:","about:","file:","mk:"]
other = ["&","=",":","?","\"","\n","%n%n%n%n%n%n%n%n%n%n%n%n","\\"]
ints = ["0","-1","127","7897","89000","808080","90928345","74326794236234","0xfffffff","ffff"]

class htmler:
 maxparams = 20
 maxtypes = 20
 def __init__(self):
  pass

 def randnum(self,finish,start=0):
  return random.randint(start,finish)

 def randstring(self):
  char = chr(self.randnum(255))
  length = self.randnum(300)
  thestring = char * length
  what = self.randnum(1)
  if what == 0:
   which = self.randnum(len(ext) - 1)
   thestring += ext[which]
  elif what == 1:
   which = self.randnum(len(pre) - 1)
   thestring = pre[which] + thestring
  else:
   return
  return thestring
 
 def makestring(self):
  what = self.randnum(2)
  if what == 0:
   return self.randstring()
  elif what == 1:
   return ints[self.randnum(len(ints) - 1)]
  elif what == 2:
   return other[self.randnum(len(other) - 1)]
   
 def loop(self):
  string = ""
  i = self.randnum(len(types) - 1)
  t = types[i]
  arr = data[t]
  string += "<%s " % types[i]
  z = 0
  for z in range(0,self.maxparams):
   badparam = arr[self.randnum(len(arr) - 1)]
   badstring = self.makestring()
   string += "%s=%s " % (badparam,badstring)
  string += ">\n"
  return string

 def main(self):
  page = ""
  for k in range(0,self.maxtypes):
   page += self.loop()
  return page

if __name__ == '__main__':
 h = htmler()
 count = 0
 while 1:
  shiz = "<HEAD>\n<META HTTP-EQUIV=\"Refresh\" content=\"0;URL=%d.html\">\n" % (count + 1)
  hehe = h.main()
  shiz += hehe
  print "count -> %d" % count
  file = open("html1/%d.html" % count,"w")
  file.write(shiz)
  file.close()
  count += 1 
