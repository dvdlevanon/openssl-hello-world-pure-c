#!/bin/bash

export message=$1
[ -z "$message" ] && message="Secure client"

echo -n "$message" | openssl s_client -CAfile cert.pem -quiet -connect localhost:33333
