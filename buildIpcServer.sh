#!/bin/sh

EXE="ipcServer"

CCFILES="\
  rapidPrototype/ipcServer.cc \
  src/IpcMessageQueue.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

