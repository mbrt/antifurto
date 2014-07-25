#!/bin/bash

DIR=/tmp/antifurto
OUT_FILE=./images/img
IMAGES=($DIR/live0.jpg $DIR/live1.jpg $DIR/live2.jpg)

COUNTER=0
while true; do
    for IMG in "${IMAGES[@]}"; do
        echo $COUNTER ${IMG}
        let "COUNTER=$COUNTER+1"
        cat ${IMG} > $OUT_FILE$COUNTER.jpg
    done
done
