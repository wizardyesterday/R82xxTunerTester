#************************************************************************
# This makefile builds the programs necessary for building the test
# system for (presently) generating IF gain data from an rtl-sdr
# radio dongle. The capability has been provided to build one
# application at a time, a selected group of applicationa at a time,
# or all of the applications.
#
# The invocation and appropriate command line options follow:
#
# 1. make clients. All test clients are buiilt.

# 2. make servers. All severs are built including the control application
#  (not really a server) and the support programs that are needed by
#  the servers. This all that is need to run the test system.
#
# 3. make all. All programs are built for which 'clients' and 'servers'
#    builds.
#************************************************************************

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# Generic defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
CPP = g++
CFLAGS = -O0 -g -I include
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# End of Generic defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# Server defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

# Coorsinator, the main control program, so I put it here.
COORDINATORTARGET = coordinator
COORDINATORFILES = src/coordinator.cc src/IpcMessageQueue.cc 

## Radio server.
RSERVERTARGET = radioServer
RSERVERFILES = src/radioServer.cc src/TcpClient.cc src/IpcMessageQueue.cc 

# Spectrum server.
SSERVERTARGET = spectrumServer
SSERVERFILES = src/spectrumServer.cc src/IpcMessageQueue.cc 

# Spectrum processor, not really  a server, but needed by
# the spectrumServer
SPROCESSORTARGET = spectrumProcessor
SPROCESSORFILES = src/spectrumProcessor.cc src/SpectrumProcessor.cc

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# End of server defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# Test client defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

# Radio client.
RCLIENTTARGET = radioClient
RCLIENTFILES = test/radioClient.cc src/IpcMessageQueue.cc 

# Spectrum client.
SCLIENTTARGET = spectrumClient
SCLIENTFILES = test/spectrumClient.cc src/IpcMessageQueue.cc 

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# End of test client defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


all: servers clients

servers: coordinator radioServer spectrumServer spectrumProcessor

clients: radioClient spectrumClient


coordinator:
	$(CPP) $(CFLAGS) -o $(COORDINATORTARGET) $(COORDINATORFILES)

radioServer:
	$(CPP) $(CFLAGS) -o $(RSERVERTARGET) $(RSERVERFILES)

spectrumServer:
	$(CPP) $(CFLAGS) -o $(SSERVERTARGET) $(SSERVERFILES)

spectrumProcessor:
	$(CPP) $(CFLAGS) -o $(SPROCESSORTARGET) $(SPROCESSORFILES) -l fftw3

radioClient:
	$(CPP) $(CFLAGS) -o $(RCLIENTTARGET) $(RCLIENTFILES)

spectrumClient:
	$(CPP) $(CFLAGS) -o $(SCLIENTTARGET) $(SCLIENTFILES)

clean:
	rm -f $(RSERVERTARGET) $(SSERVERTARGET) $(SPROCESSORTARGET) \
	$(COORDINATORTARGET) $(RCLIENTTARGET) $(SCLIENTTARGET)

