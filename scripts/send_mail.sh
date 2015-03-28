#!/bin/bash

if [ "$#" -lt 5 ]; then
	echo "usage $0 <sender_name> <sender_address> <subject> <msg> [<dest]*"
	exit 1
fi

SENDER_NAME=$1
SENDER_ADDRESS=$2
SUBJECT=$3
MESSAGE=$4
DEST=${@:5:$#}

echo $MESSAGE | mail -a "From: $SENDER_NAME <$SENDER_ADDRESS>" -s $SUBJECT $DEST
exit $?
