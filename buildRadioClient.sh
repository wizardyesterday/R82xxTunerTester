#!/bin/sh

EXE="radioClient"

CCFILES="\
  test/radioClient.cc \
  src/IpcMessageQueue.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

