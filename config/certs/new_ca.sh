#!/bin/bash
rm -rf ca
mkdir ca ca/private ca/certs ca/crl ca/newcerts
touch ca/index.txt ca/serial ca/crlnumber
echo '01' > ca/serial
echo '01' > ca/crlnumber
openssl req -config ca/openssl.cnf -new -x509 -extensions v3_ca -keyout ca/private/ca.key -out ca/certs/ca.crt -days 7300
chmod 0400 ca/private/ca.key
