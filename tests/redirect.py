import re
import requests
import sys

# global variables

rdStatusCodes = ['301', '302', '303', '307', '308']
responseHeaders = ["Content-Length", "Content-Type", "Date", "Location", "Server"]

class bcolors :

    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def statusCodesBody(code) :

	statusCodesBody = {
	301: "<html>\r\n<head><title>301 Moved Permanently</title></head>\r\n<body>\r\n<center><h1>301 Moved Permanently</h1></center>\r\n<hr><center>Windmill</center></body></html>",
	302: "<html>\r\n<head><title>302 Found</title></head>\r\n<body>\r\n<center><h1>302 Found</h1></center>\r\n<hr><center>Windmill</center></body></html>",
	303: "<html>\r\n<head><title>303 See Other</title></head>\r\n<body>\r\n<center><h1>303 See Other</h1></center>\r\n<hr><center>Windmill</center></body></html>",
	307: "<html>\r\n<head><title>307 Temporary Redirect</title></head>\r\n<body>\r\n<center><h1>307 Temporary Redirect</h1></center>\r\n<hr><center>Windmill</center></body></html>",
	308: "<html>\r\n<head><title>308 Permanent Redirect</title></head>\r\n<body>\r\n<center><h1>308 Permanent Redirect</h1></center>\r\n<hr><center>Windmill</center></body></html>",
	403: "<html>\r\n<head><title>403 Forbidden</title></head>\r\n<body>\r\n<center><h1>403 Forbidden</h1></center>\r\n<hr><center>Windmill</center></body></html>"
	}

	return statusCodesBody[code]

def checkHeader(response, code) :

	if response.status_code == int(code) :
		print(bcolors.OKBLUE + "statuscode [" + code + "]" + bcolors.OKGREEN + " [OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "statuscode [" + code + "]" + " [FAILED]" + bcolors.ENDC)

	for header in responseHeaders :

		try :
			response.headers[header]
			if header == "Content-Length" :
				if int(response.headers[header]) == len(statusCodesBody(int(code))) :
					print(bcolors.OKBLUE + header + " [" + response.headers[header] + "] " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
				else :
					print(bcolors.FAIL + header + " [FAILED]" + bcolors.ENDC)
			elif header == "Content-Type" :
				if response.headers[header] == "text/html" :
					print(bcolors.OKBLUE + header + " [" + response.headers[header] + "] " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
				else :
					print(bcolors.FAIL + header + " [FAILED]" + bcolors.ENDC)
			elif header == "Server" :
				if response.headers[header] == "Windmill" :
					print(bcolors.OKBLUE + header + " [" + response.headers[header] + "] " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
				else :
					print(bcolors.FAIL + header + " [FAILED]" + bcolors.ENDC)
			else :
				print(bcolors.OKBLUE + header + bcolors.OKGREEN + " [OK]" + bcolors.ENDC)
		except :
			print(bcolors.WARNING + "header " + header + " not found " + "[WARNING]" + bcolors.ENDC)

def checkBody(response, code) :

	if (response.content.decode('utf-8') == statusCodesBody(int(code))) :
		print(bcolors.OKBLUE + "body " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "body [FAILED]" + bcolors.ENDC)

def getRequest() :

	i = 0
	for code in rdStatusCodes :
		print ("\nGET REQUEST: http://localhost/" + rdStatusCodes[i] + "\n")
		try :
			response = requests.get('http://localhost/' + rdStatusCodes[i])
		except :
			print(bcolors.FAIL + "requests.get() [failed] => check if webserv is runnning and if the correct config file is used" + bcolors.ENDC)
		if (response.status_code == 200) :
			print(bcolors.OKBLUE + "location [" + str(response.status_code) + "] " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
		else :
			print(bcolors.OKBLUE + "location [" + str(response.status_code) + "] " + bcolors.WARNING + "[WARNING]" + bcolors.ENDC) # https://www.example.com could be down
		if len(response.history) < 1 :
			print(bcolors.FAIL + "redirect failed" + bcolors.ENDC)
		elif len(response.history) > 1 :
			print(bcolors.WARNING + "unexpected amount of redirects" + bcolors.ENDC)
		checkHeader(response.history[0], code)
		checkBody(response.history[0], code)
		print("\n-- END --")
		i+=1

def deleteRequest() :

	code = 403

	print ("\nDELETE REQUEST: http://localhost:8080/301/delete\n")

	try :
		response = requests.delete("http://localhost:8080/302/delete")
	except :
		print(bcolors.FAIL + "requests.get() [failed] => check if webserv is runnning and if the correct config file is used" + bcolors.ENDC)	

	if len(response.history) != 0 :
		print(bcolors.FAIL + "unexpected redirects [FAILED]" + bcolors.ENDC)

	if (response.status_code == code) :
		print(bcolors.OKBLUE + "response [" + str(response.status_code) + "]" + bcolors.OKGREEN + " [OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "response [" + str(response.status_code) + "]" + " [FAILED]" + bcolors.ENDC)
	checkHeader(response, str(response.status_code))
	checkBody(response, code)


def postRequest() :

	code = 403

	print ("\nPOST REQUEST: http://localhost:8080/301/post\n")

	try :
		response = requests.post("http://localhost:8080/302/post")
	except :
		print(bcolors.FAIL + "requests.get() [failed] => check if webserv is runnning and if the correct config file is used" + bcolors.ENDC)	

	if len(response.history) != 0 :
		print(bcolors.FAIL + "unexpected redirects [FAILED]" + bcolors.ENDC)

	if (response.status_code == code) :
		print(bcolors.OKBLUE + "response [" + str(response.status_code) + "]" + bcolors.OKGREEN + " [OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "response [" + str(response.status_code) + "]" + " [FAILED]" + bcolors.ENDC)
	checkHeader(response, str(response.status_code))
	checkBody(response, code)

def runTests() :

	getRequest()
	deleteRequest()
	postRequest()

def main():
	runTests()

if __name__ == "__main__" :
	try :
		main()
	except :
		sys.exit()
