#!/bin/bash
# SHA512 testcase -- all 3 layers.
#
# see: http://openssl.6102.n7.nabble.com/create-certificate-chain-td44046.html
#      http://superuser.com/questions/126121/how-to-create-my-own-certificate-chain
LEN=${LEN:-2048}
OUT=${DIR:-ca}

mkdir -p $OUT
pushd $OUT

# create a root.
openssl req -new -x509 -nodes -out ca.crt -keyout ca.key -subj /CN=MBRoot -newkey rsa:$LEN -sha512 || exit 1

# create an intermediate & sign
openssl req -new -nodes -out ca-int.req -keyout ca-int.key -subj /CN=MBintermediate -newkey rsa:$LEN -sha512 || exit 1
openssl x509 -req -in ca-int.req -CAkey ca.key -CA ca.crt -days 7300 -set_serial $RANDOM -sha512 -out ca-int.crt || exit 1

# chain
#
cat ca.crt ca-int.crt > ca-all.crt
popd
