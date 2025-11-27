#!/bin/sh

g++ -O2 -Iinclude -o spectrumProcessor  src/spectrumProcessor.cc src/SpectrumProcessor.cc -l fftw3

