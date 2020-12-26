#!/bin/sh
# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.
#
# Script which can be used to easily run Starfighter in-place without
# installing on POSIX systems, but not on MacOS.

SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd $SCRIPTPATH
./src/starfighter "$@"