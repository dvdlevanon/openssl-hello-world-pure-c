#!/bin/bash

make all || exit 1
./ssl-server
