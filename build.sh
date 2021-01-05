#!/bin/bash

mkdir build
cd build
qmake ..
make
mkdir ../Ship
cp *.so ../Ship