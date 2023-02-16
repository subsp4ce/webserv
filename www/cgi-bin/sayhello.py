import cgi, os
import cgitb; cgitb.enable()
import cgitb

def sayHello(form, statusLine, successful, message) :
	
	print(statusLine + "\r")
	print("Content-Type: text/html\r\n\r\n")
	print("<!doctype html>")
	print("<html>")
	print("<head>")
	print("<title>Say Hello!</title>")
	print("<style>")
	print("body {")
	print("background-image: linear-gradient(to right, rgb(189, 255, 203) , white); ")
	print("font-family: 'Courier New', Courier, monospace;")
	print("}")
	print("</style>")
	print("</head>")
	print("<body>")
	print("<center>")
	print("<h1>")
	if (successful) :
		print(form.getvalue('say').upper())
		print(form.getvalue('to').upper())
	else :
		print(message)
	print("</h1>")
	print("<br>")
	print("</center>")
	print("</body>")
	print("</html>")
	exit()

def main() :

	cgitb.enable()
	cgitb.enable(display=0, logdir="/logs")

	form = cgi.FieldStorage()

	requestMethod = os.getenv('REQUEST_METHOD')
	if requestMethod != "GET" :
		sayHello(form, "HTTP/1.1 502 Bad Gateway", False, "Request method must be GET")

	if not form :
		sayHello(form, "HTTP/1.1 502 Bad Gateway", False, "No form values")

	if form.getvalue('say') and form.getvalue('to') :
		sayHello(form, "HTTP/1.1 200 OK", True, "")
	else :	
		sayHello(form, "HTTP/1.1 502 Bad Gateway", False, "Different form values expected")


if __name__ == "__main__" :
		main()
