#!/bin/bash

TMPPATH=./builddir

rm -rf $TMPPATH
CC=gcc CXX=g++ CC_LD=gold CXX_LD=gold meson setup -Doptimization=3 $TMPPATH &&
ninja -C $TMPPATH
