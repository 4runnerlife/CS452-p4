#!/bin/bash
if [ -d ./build ]
then
    cd build
    make clean && make && make test
else 
    echo "Build directory not present. Run setup.sh first!"
fi