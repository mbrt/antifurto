#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage $0 <cert-name>"
    exit 1
fi
CERNAME=$1
pushd .
cd ca
mkdir -p crl
openssl ca -config openssl.cnf -revoke certs/$CERNAME.crt
openssl ca -config openssl.cnf -gencrl -out crl/ca.crl
popd
