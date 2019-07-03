#!/bin/sh
# Note: Before running, be sure to add the MXE usr/bin location to PATH

export SF_RUN_IN_PLACE=1
./configure --host=i686-w64-mingw32.static
make
mv src/starfighter.exe .
