#!/bin/sh

EXE="ipcClient"

CCFILES="\
  rapidPrototype/ipcClient.cc \
  src/IpcMessageQueue.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

