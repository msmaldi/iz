#!/bin/bash

TMPPATH=./builddir

rm -rf $TMPPATH
CC=clang CXX=clang++ CC_LD=lld CXX_LD=lld meson setup -Dwerror=true -Doptimization=3 $TMPPATH &&
ninja -C $TMPPATH
