#!/bin/bash

GREEN='\033[1;32m'
RED='\033[1;31m'
RESET='\033[0m'

printf "${GREEN}USAGE:\n\nRUN: ./webserv tests/config_files/fileupload.conf\nRUN ./fileupload POST or ./fileupload DELETE\n\n${RESET}"
read -p "Hit return to resume ..."

if [[ $1 != "POST" && $1 != "DELETE" ]]
then
	echo "Error: not a valid argument. Use [POST] or [DELETE]"
	exit
fi

for FILE in files/*;
do
	if [[ $1 == POST ]]
	then
		curl -F "file=@$FILE" http://localhost:80/cgi-bin/fileupload.py
	else
		curl -X DELETE "http://localhost/uploads/${FILE:6}"
		printf "$GREENuploads/${FILE:6} deleted $RESET\n"
	fi
	sleep 0.5
done
