#!/bin/bash

TMPPATH=./builddir

rm -rf $TMPPATH
CC=clang CXX=clang++ CC_LD=lld CXX_LD=lld meson setup -Db_coverage=true -Dwerror=true $TMPPATH &&
ninja -C $TMPPATH test &&
ninja -C $TMPPATH coverage-html
