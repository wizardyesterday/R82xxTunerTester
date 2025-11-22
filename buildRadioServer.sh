#!/bin/sh

EXE="radioServer"

CCFILES="\
  src/radioServer.cc \
  src/TcpClient.cc"

g++ -O0 -g -o $EXE -I include $CCFILES

