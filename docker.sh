#!/bin/bash

make all || exit 1
docker build -t openssl-hello-world-pure-c . || exit 1
docker run -p 33333:33333 -p 44444:44444 -it openssl-hello-world-pure-c
