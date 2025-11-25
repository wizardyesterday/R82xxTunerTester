#!/bin/sh

EXE="spectrumServer"

CCFILES="\
  src/spectrumServer.cc \
  src/IpcMessageQueue.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

