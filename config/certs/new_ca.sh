#!/bin/bash
rm -rf ca
mkdir ca ca/private ca/certs ca/crl ca/newcerts
touch ca/index.txt
touch ca/serial
openssl req -config ca/openssl.cnf -new -x509 -extensions v3_ca -keyout ca/private/ca.key -out ca/certs/ca.crt -days 7300
chmod 0400 ca/private/ca.key
