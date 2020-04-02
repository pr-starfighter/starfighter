#!/bin/sh
# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.
#
# Helper script to cross-compile for Windows with MXE.
# Note: Before running, be sure to add the MXE usr/bin location to PATH.

./configure --host=i686-w64-mingw32.static SF_RUN_IN_PLACE=1 SF_NOWARN=1
make
mv src/starfighter.exe .
