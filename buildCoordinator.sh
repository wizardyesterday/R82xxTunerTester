#!/bin/sh

EXE="coordinator"

CCFILES="\
  src/coordinator.cc \
  src/TcpClient.cc
  src/IpcMessageQueue.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

