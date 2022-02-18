#!/bin/sh
oclint -p build/debug -enable-clang-static-analyzer -enable-global-analysis include/**/*.hh source/**/*.cc -rc LONG_LINE=120 -report-type html -o /tmp/oclint.html
firefox /tmp/oclint.html
