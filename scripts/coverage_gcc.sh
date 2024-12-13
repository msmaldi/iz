#!/bin/bash

TMPPATH=./builddir

rm -rf $TMPPATH
CC=gcc CXX=g++ CC_LD=gold CXX_LD=gold meson setup -Db_coverage=true -Dwerror=true $TMPPATH &&
ninja -C $TMPPATH test &&
ninja -C $TMPPATH coverage-html
