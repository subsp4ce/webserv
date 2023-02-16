#!/usr/bin/python

import cgi, os
import cgitb; cgitb.enable()
import cgitb

def response(statusline, statuscode, message, successful, fn) :

	print(statusline + "\r")
	if (successful) :
		print("Location: " + "http://localhost:" + os.getenv('SERVER_PORT') + "/uploads/" + fn + "\r")
	print("Content-Type: text/html\r\n\r\n")
	print("<!doctype html>")
	print("<html>")
	print("<head>")
	print("<title>File Upload</title>")
	print("<style>")
	print("body {")
	print("background-image: linear-gradient(to right, rgb(177, 238, 255) , white);")
	print("font-family: 'Courier New', Courier, monospace;")
	print("}")
	print("</style>")
	print("</head>")
	print("<body>")
	print("<center>")
	print("<h1>")
	print(statuscode)
	print("</h1>")
	print("<b>")
	print(message)
	print("</b>")
	print("<br>")
	if (successful) :
		print("<a href=\"http://localhost:" + os.getenv('SERVER_PORT') + "/uploads/" + fn )
		print("\">View file</a>")
		print("<br>")
		print("<br>")
		print("or")
		print("<br>")
		print("<br>")
	print("<a href=\"http://localhost\">Go back to homepage</a>")
	print("</center>")
	print("</body>")
	print("</html>")
	exit()

def uploadFile() :

	cgitb.enable()
	cgitb.enable(display=0, logdir="/logs")

	requestMethod = os.getenv('REQUEST_METHOD')
	if requestMethod != "POST" :
		response("HTTP/1.1 502 Bad Gateway", "502 Bad Gateway", "Request method must be POST", False, "")

	upload_dir = os.getenv('UPLOAD_DIR')

	form = cgi.FieldStorage()

	try:
		fileitem = form['file']
	except:
		response("HTTP/1.1 502 Bad Gateway", "502 Bad Gateway", "File key in form not provided", False, "")

	if fileitem.filename:

		fn = os.path.basename(fileitem.filename)
		try:
			open("../../" + upload_dir + fn, 'wb').write(fileitem.file.read())
			response("HTTP/1.1 201 Created", 'The file "' + fn + '" was uploaded successfully', "", True, fn)
		except IOError:
			response("HTTP/1.1 502 Bad Gateway", "502 Bad Gateway", "No file was uploaded", False, "")

def main() :

	uploadFile()

if __name__ == "__main__" :
		main()
