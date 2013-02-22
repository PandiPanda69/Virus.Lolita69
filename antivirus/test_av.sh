#!/bin/bash

BIN=./bin/
ANTIVIRUS=antivirus_packed
LS=/bin/ls

E_OK=0
E_USAGE=1
E_NOT_FOUND=2

if [ $# -ne 1 ]
then
	echo "Usage:"
	echo "$0 <directory_to_scan>"

	exit $E_USAGE
fi


if [ -e $1 ]
then
	for current in `$LS $1`
	do
		if [ -d "$1/$current" ]
		then
			./$0 $1/$current
		else
			echo $current
			$BIN$ANTIVIRUS $1/$current
		fi
	done
else
	echo "Directory $1 not found."
	exit $E_NOT_FOUND
fi


exit $E_OK
