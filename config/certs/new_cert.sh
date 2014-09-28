#!/bin/bash
LEN=${LEN:-2048}

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <ca-int-name> <ca-all.crt> <out-name> <cn>"
    exit 1
fi

function check_file {
    if [ ! -f $1 ]; then
        echo "Missing $1 file"
        exit 1
    fi
}

CA_INT_CRT="$1.crt"
CA_INT_KEY="$1.key"
CA_ALL_CRT=$2
WHO=$3
CN=$4

check_file $CA_INT_KEY
check_file $CA_INT_CRT

# create a request
openssl req -new -out $WHO.req -keyout $WHO.key -nodes -newkey rsa:$LEN -subj /CN=$CN/emailAddress=$CN@mail.com  || exit 1

# sign the request
openssl x509 -req -in $WHO.req -CAkey $CA_INT_KEY -CA $CA_INT_CRT -days 3650 -set_serial $RANDOM -sha512 -out $WHO.crt || exit 1

# create some convenience formats
#
openssl x509 -in $WHO.crt -out $WHO.pem -outform PEM || exit 1
#openssl pkcs12 -export -out $WHO.p12 -in $WHO.crt -inkey $WHO.key -chain -CAfile $CA_ALL_CRT -password pass:$PASS || exit 1
openssl pkcs12 -export -out $WHO.p12 -in $WHO.crt -inkey $WHO.key -chain -CAfile $CA_ALL_CRT || exit 1
openssl pkcs12 -in $WHO.p12 -out $WHO-private.pem -nodes || exit 1
