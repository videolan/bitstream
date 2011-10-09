#!/bin/sh
# Script to test PSI generation and parsing
# Copyright (C) 2011 Unix Solutions Ltd.
#
# License: MIT
# Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
#

rm dvb_gen_si dvb_print_si
make local

./dvb_gen_si | ./dvb_print_si        > dvb_print_si.output.txt
./dvb_gen_si | ./dvb_print_si -x xml > dvb_print_si.output.xml

git diff dvb_print_si.output.txt dvb_print_si.output.xml
