import sys
import requests
from utils import colors

def checkHeaderFields(statusCode, response) :

	if response.status_code == int(statusCode) :
		print(colors.fg.blue + "statuscode [" + str(statusCode) + "]" + colors.fg.green + " [OK]" + colors.reset)
	else :
		print(colors.fg.red + "statuscode " + str(response.status_code) + " incorrect, " + str(statusCode) + " expected" + colors.reset)
	
	# check content length

	print('\n')

def checkBody(body, fileContent) :

	if body == fileContent :
		print(colors.fg.blue + "body content identical to file content " + colors.fg.green + "[OK]" + colors.reset)
	else :
		print(colors.fg.red + "body not identical to file content [FAILED]" + colors.reset)

def getFileContent(fn) :

	try :
		fn = open(fn, 'r')
	except :
		print(colors.fg.red + fn + " failed to open" + colors.reset)
	data = fn.read()
	return data

def runTests() :

	hostNames = ["salmari.com", "kahlua.com", "tanqueray.com", "mezcal.com"]
	for host in hostNames :
		try :
			response = requests.get('http://localhost:80', headers={'Host': host})
		except :
			print(colors.fg.red + "requests.get failed" + colors.reset)
		fn = host[:-4] + ".html"
		data = getFileContent('www/resolvehost00/' + fn)
		checkBody(response.content.decode('utf-8'), data)
		checkHeaderFields(200, response)

def main():

	print(colors.fg.green + "\n-- resolvehost00 --\n" + colors.reset)
	runTests()

if __name__ == "__main__" :
	# try :
		main()
	# except :
		sys.exit()
