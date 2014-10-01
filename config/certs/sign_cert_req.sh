#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage $0 <cert-name>"
    exit 1
fi
CERNAME=$1
pushd .
cd ca
openssl ca -config openssl.cnf -policy policy_anything -out certs/$CERNAME.crt -infiles $CERNAME.csr
cat certs/$CERNAME.crt private/$CERNAME.key > private/$CERNAME-key-cert.pem
chmod 0400 private/$CERNAME.key private/$CERNAME-key-cert.pem
popd
