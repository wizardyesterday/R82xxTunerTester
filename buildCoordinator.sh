#!/bin/sh

EXE="coordinator"

CCFILES="\
  src/coordinator.cc \
  src/TcpClient.cc
  src/IpcMessageQueue.cc"

g++ -O2 -o $EXE -I include $CCFILES
#g++ -O0 -g -o $EXE -I include $CCFILES

