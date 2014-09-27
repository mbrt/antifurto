#!/bin/bash
# SHA512 testcase -- all 3 layers.
#
# see: http://openssl.6102.n7.nabble.com/create-certificate-chain-td44046.html
#      http://superuser.com/questions/126121/how-to-create-my-own-certificate-chain
LEN=${LEN:-2048}

# create a root.
 openssl req -new -x509 -nodes -out ca.crt -keyout ca.key -subj /CN=DaRoot -newkey rsa:$LEN -sha512 || exit 1

# create an intermediate & sign
openssl req -new -nodes -out ca-int.req -keyout ca-int.key -subj /CN=Zintermediate -newkey rsa:$LEN -sha512 || exit 1
openssl x509 -req -in ca-int.req -CAkey ca.key -CA ca.crt -days 20 -set_serial $RANDOM -sha512 -out ca-int.crt || exit 1

# chain
#
cat ca.crt ca-int.crt > ca-all.crt

for who in alice bob charlie eve dave fred
do
    # create a request
    openssl req -new -out $who.req -keyout $who.key -nodes -newkey rsa:$LEN -subj /CN=$who/emailAddress=$who@mail.com  || exit 1

    # sign the request
    openssl x509 -req -in $who.req -CAkey ca-int.key -CA ca-int.crt -days 10 -set_serial $RANDOM -sha512 -out $who.crt || exit 1

    # create some convenience formats
    #
    openssl x509 -in $who.crt -out $who.pem -outform PEM || exit 1
    openssl pkcs12 -export -out $who.p12 -in $who.crt -inkey $who.key -chain -CAfile ca-all.crt -password pass:$PASS || exit 1
done 
