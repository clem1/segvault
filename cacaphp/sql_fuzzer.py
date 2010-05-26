#!/usr/bin/python
#
#
# Usage : $ ./sql_fuzzer.py -s "/var/www/phpBB2/" 
#                           -h "localhost" 
#                           -u "/phpBB2/search.php" 
#                           -q "mode=result" 
#                           -d "search_keywords=fds&search_terms=any&search_author=&search_forum=-1"
#                              "&search_time=0&search_fields=all&search_cat=-1&sort_by=0&sort_dir=DESC"
#                              "&show_results=topics&return_chars=200"
#



import os, sys, re, httplib, urllib, getopt



#
# return the list of files in @path
# @ret = [..., "path/dir/file", ...]
#
def list_dir(path):
  file_list = []

  if path[len(path) - 1] != '/':
    path = path + "/"

  for file_name in os.listdir(path):
    file_path = os.path.join(path, file_name)
    if file_path[len(path) - 3] == "/./" or file_path[len(path) - 4] == "/../":
      continue
    if os.path.isfile(file_path):
      file_list.append(file_path)
    else:
      file_list.extend(list_dir(file_path))

  return file_list



#
# return the list of variables from files in @file_list
# @ret = [..., "$id", ...]
#
def get_vars(file_list, meth):
  ret = []
  method = {"GET" : ["HTTP_GET_VARS", "_GET", "REQUEST"], "POST" : ["HTTP_POST_VARS", "_POST", "REQUEST"], "COOKIE" : ["HTTP_COOKIE_VARS", "_COOKIE", "REQUEST"]}

  for file_name in file_list:
    try:
	   fp = open(file_name, "r")
    except:
      break;
    for line in fp.readlines():
      for prefix in method[meth]:
        m = re.compile("\$"+prefix+"\[[:space:]*'([^\]]+)'[:space:]*]"
                       "|\$"+prefix+"\[[:space:]*\"([^\]]+)]\"[:space:]*"
                       "|\$"+prefix+"\[[:space:]*([^\]]+)[:space:]*\]")
        for varss in m.findall(line):
          for var in varss:
            if var not in ret:
              ret.append(re.sub("\"", "", var))
    fp.close()

  return ret



#
# show a request
#
def show_request(host, url, query, data, cookie):
  print "********************************************************************************"
  if data != "":
    line = "POST "
  else:
    line = "GET "
  line = line+url+" HTTP/1.0"
  print line
  print "Host: "+host
  print "Connection: close"
  if cookie != "":
    print "Cookie: "+cookie
  if data != "":
    print "Content-type: application/x-www-form-urlencoded"
    print "Content-length: "+str(len(data))+"\n"
    print data
  print "********************************************************************************"



#
# send the request @url to @host
#
def send_request(host, url, query, data, cookie):
  query = urllib.urlencode(query)
  data = urllib.urlencode(data)
  if query != "":
    url = url+"?"+query
  
  h = httplib.HTTP(host)
  if data == "":
    h.putrequest("GET", url)
  else:
    h.putrequest("POST", url)
    h.putheader("Content-type", "application/x-www-form-urlencoded")
    h.putheader("Content-length", "%d" % len(data))

  h.putheader("Host", host)
  h.putheader("Connection", "close")
  if cookie != "":
    h.putheader("Cookie", cookie)
  h.endheaders()
  if data != "":
    h.send(data)

  (errcode, errmsg, headers) = h.getreply()
  if errcode != 200 and errcode != 302:
    print "Request failed : "+str(errcode)+", "+errmsg+ "("+url+")"

  f = h.getfile()
  for line in f.readlines():
    m = re.search("Set-Cookie:", line)
    if not m:
      m = re.search("sql|error|warning|<coin>|WARNING|ERROR", line.lower())
      if m and not re.search("errorwrap", line) and not re.search("sqlmain", line) and not re.search("sqltop", line):
        print "SQL error : "+line
        show_request(host, url, query, data, cookie)
        print ""
  f.close()



#
#
#
def fuzz(var_list, method, host, url, query={}, data={}, cookie=""):
  bad_values = ["<coin>", "'", "\'", "\0", "-1", "`", ")", "\\", "|", "\n"]
  save = ""
  
  #if var_list[len(var_list)-1] == "":
  #  del(var_list[len(var_list)-1])

  for var in var_list:
    for val in bad_values:
      if method == "GET":
        if query.has_key(var):
          save = query[var]
        query[var] = val
        send_request(host, url, query, data, cookie)
        del(query[var])
        if save:
          query[var] = save
          save = ""
      elif method == "POST":
        if data.has_key(var):
          save = data[var]
        data[var] = val
        send_request(host, url, query, data, cookie)
        del(data[var])
        if save:
          data[var] = save
          save = ""
      elif method == "COOKIE":
        send_request(host, url, query, data, cookie+"; "+var+"="+val)



#
# urldecode
#
def urldecode(url):
  res = {}
  url = url.replace('+', ' ')
  l = url.split("%")
  url = l[0]
  for i in range(1, len(l)):
    c = l[i]
    l[i] = chr(int(c[:2], 16)) + c[2:]
    url = url +l[i]
  for v in url.split("&"):
    (key, value) = v.split("=")
    res[key] = value
  return res



#
# usage
#
def usage():
  print "./sql_fuzzer.py -s <source> -h <host> -u <url> -q [query] -d [data] -c [cookie]" # -w [words]"
  print "Options :"
  print "  -s\tPHP sources directory\t\t(/var/www/phpBB2)"
  print "  -h\thost\t\t\t\t(localhost)"
  print "  -u\tabsolute URL\t\t\t(/forum/search.php)"
  print "  -q\tRequest query\t\t\t(sid=123)"
  print "  -d\tPost content\t\t\t(search_keywords=hello)"
  print "  -c\tCookies\t\t\t\t(phpbb2mysql_data=123)"
  #print "  -w\tWords we don't want to match\t(<p>Error</p>)"

  sys.exit(0)




#
# main
#
def main(argv):
  source = ""
  host = ""
  url = ""
  data = {}
  cookie = ""
  query = {}

  try:
    opts, args = getopt.getopt(argv, "s:h:u:q:d:c:w:")
  except getopt.GetoptError:
    usage()
  for opt, arg in opts:
    if opt == "-s":
      source = arg
    elif opt == "-h":
      host = arg
    elif opt == "-u":
      url = arg
    elif opt == "-q":
      query = urldecode(arg)
    elif opt == "-d":
      data = urldecode(arg)
    elif opt == "-c":
      cookie = arg
  if not source or not host or not url:
    usage()

  file_list = list_dir(source)
  for method in ["GET", "POST", "COOKIE"]:
    fuzz(get_vars(file_list, method), method, host, url, query, data, cookie)


if __name__ == "__main__":
  main(sys.argv[1:])
  
