#!/bin/bash

TMPPATH=/tmp/iz/buildcov

rm -rf $TMPPATH
meson setup -Db_coverage=true -Dwerror=true -Db_coverage=true $TMPPATH &&
ninja -C $TMPPATH test &&
ninja -C $TMPPATH coverage-html
