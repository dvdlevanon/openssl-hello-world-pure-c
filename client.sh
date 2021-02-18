#!/bin/bash

export message=$1
[ -z "$message" ] && message="Non secure client"

echo -n "$message" | nc -v localhost 44444
