#!/bin/bash

sudo tcpdump -A -n -i lo "(src port 44444 or src port 33333 or dst port 44444 or dst port 33333)" | grep -v ", length 0"
