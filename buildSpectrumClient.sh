#!/bin/sh

EXE="spectrumClient"

CCFILES="\
  test/spectrumClient.cc \
  src/IpcMessageQueue.cc"

g++ -O2 -o $EXE -I include $CCFILES
#g++ -O0 -g -o $EXE -I include $CCFILES

