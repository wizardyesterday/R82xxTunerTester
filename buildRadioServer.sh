#!/bin/sh

EXE="radioServer"

CCFILES="\
  src/radioServer.cc \
  src/IpcMessageQueue.cc \
  src/TcpClient.cc"

g++ -O2 -o $EXE -I include $CCFILES
#g++ -O0 -g -o $EXE -I include $CCFILES

