#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage $0 <cert-name>"
    exit 1
fi
CERNAME=$1
openssl req -config ca/openssl.cnf -new -nodes -keyout ca/private/$CERNAME.key -out ca/$CERNAME.csr -days 3650
