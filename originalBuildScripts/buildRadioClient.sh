#!/bin/sh

EXE="radioClient"

CCFILES="\
  test/radioClient.cc \
  src/IpcMessageQueue.cc"

g++ -O2 -o $EXE -I include $CCFILES
#g++ -O0 -g -o $EXE -I include $CCFILES

