#!/bin/bash
# this is an example on how to send an SMS with curl and cardboardfish service

if [ "$#" -ne 3 ]; then
    echo "usage $0 <receiver> <sender> <message>"
    exit 1
fi

RECEIVER=$1
SENDER=$2
MSG=$3

USER="user111"
PWD="mypwd"
ENCODED_MSG="004D0042"

SITE="http://sms1.cardboardfish.com:9001/HTTPSMS"
REQUEST="S=H&UN=$USER&P=$PWD&DA=39$RECEIVER&SA=$SENDER&M=$ENCODED_MSG&DC=4"
curl $SITE -d$REQUEST
