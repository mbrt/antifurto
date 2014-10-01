#!/bin/bash
mkdir -p ca/private ca/certs
openssl req -config ca/openssl.cnf -new -x509 -extensions v3_ca -keyout ca/private/ca.key -out ca/certs/ca.crt -days 7300
chmod 0400 ca/private/ca.key
