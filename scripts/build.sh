#!/bin/bash

TMPPATH=./builddir

rm -rf $TMPPATH
meson setup -Dwerror=true -Doptimization=3 $TMPPATH &&
ninja -C $TMPPATH
