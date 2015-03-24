#!/bin/bash

if [ "$#" -lt 3 ]; then
	echo "usage $0 <subject> <msg> [<dest]*"
	exit 1
fi

SUBJECT=$1
MESSAGE=$2
DEST=${@:3:$#}

echo $MESSAGE | mail -s $SUBJECT $DEST
exit $?
