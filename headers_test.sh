#!/bin/sh
# Test if each header can be used standalone
# Copyright (C) 2011 Unix Solutions Ltd.
#
# License: MIT
# Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
#

HEADER_LIST="dvb/sim.h ietf/rtp.h"
# Find is used in such a way in order to work on OS X as well as Linux
# If you want to test only particular headers comment the line bellow
HEADER_LIST=$(find * | grep \\.h$ | sort | sed -e 's|^\./||')

for HDR in $HEADER_LIST
do
    test_file=$(echo $HDR | sed -e 's|/|_|g;s|\.h$||')
    echo "Testing: $HDR"
    printf "#include \"$HDR\"\n\nint main(void) { return 0; }\n" > $test_file.cpp
    gcc -I.. -Werror -Wall -Wextra -Wno-unused -Wno-sign-compare -Wformat-security $test_file.cpp -o $test_file
    [ $? != 0 ] && exit 1
    rm $test_file $test_file.cpp
done
