from fileinput import filename
import requests
import sys
import time

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

responseHeaders = ["Content-Length", "Content-Type", "Date", "Server"]

def checkHeader(response, code, len) :

	if response.status_code == int(code) :
		print(bcolors.OKBLUE + "statuscode [" + code + "]" + bcolors.OKGREEN + " [OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "statuscode [" + code + "]" + " [FAILED]" + bcolors.ENDC)

	for header in responseHeaders :

		try :
			response.headers[header]
			if header == "Content-Length" :
				if int(response.headers[header]) == len :
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

def checkBody(response, data) :

	if response.content.decode('utf-8') == data :
		print(bcolors.OKBLUE + "body " + bcolors.OKGREEN + "[OK]" + bcolors.ENDC)
	else :
		print(bcolors.FAIL + "body [FAILED]" + bcolors.ENDC)

def serverRoot() :

	fileName = "www/kapouet/index.html"
	response = requests.get('http://localhost/')
	try :
		f = open(fileName)
	except :
		print(bcolors.FAIL + fileName + " failed to open" + " [FAILED]" + bcolors.ENDC)
	print ("\nSERVER ROOT: http://localhost\n")
	data = f.read()
	fileLen = len(data)
	checkHeader(response, "200", fileLen)
	checkBody(response, data)
	f.close

def locationRoot() :

	fileName = "www/kapouet/kapouet/index.html"
	response = requests.get('http://localhost/kapouet')
	try :
		f = open(fileName)
	except :
		print(bcolors.FAIL + fileName + " failed to open" + " [FAILED]" + bcolors.ENDC)
	print ("\nLOCATION ROOT: http://localhost/kapouet\n")
	data = f.read()
	fileLen = len(data)
	checkHeader(response, "200", fileLen)
	checkBody(response, data)
	f.close

def alias() :
	
	fileName = "www/pouic/toto/pouet/index.html"
	response = requests.get('http://localhost:8080/kapouet/pouic/toto/pouet')
	try :
		f = open(fileName)
	except :
		print(bcolors.FAIL + fileName + " failed to open" + " [FAILED]" + bcolors.ENDC)
	print ("\nALIAS: http://localhost:8080/kapouet\n")
	data = f.read()
	fileLen = len(data)
	checkHeader(response, "200", fileLen)
	checkBody(response, data)
	f.close

def runTests() :

	print(bcolors.OKGREEN + "\nUSAGE:\n\n1) RUN: ./webserv tests/config_files/root.conf\n2) RUN ./root.py\n\n" + bcolors.ENDC)

	input("Press Enter to continue...")

	serverRoot()
	time.sleep(1)
	locationRoot()
	time.sleep(1)
	alias()

def main():
	runTests()

if __name__ == "__main__" :
	try :
		main()
	except :
		sys.exit()
