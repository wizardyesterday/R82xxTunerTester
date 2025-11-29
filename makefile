#************************************************************************
#************************************************************************

CPP = g++
CFLAGS = -O0 -g -I include

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# Server defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

## Radio server.
RSERVERTARGET = radioServer
RSERVERFILES = src/radioServer.cc src/TcpClient.cc src/IpcMessageQueue.cc 

# Spectrum server.
SSERVERTARGET = spectrumServer
SSERVERFILES = src/spectrumServer.cc src/IpcMessageQueue.cc 

# Spectrum processor.
SPROCESSORTARGET = spectrumProcessor
SPROCESSORFILES = src/spectrumProcessor.cc src/SpectrumProcessor.cc

#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# End of server defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# Test client defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
# End of test client defines.
#/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/


all: radioServer spectrumServer spectrumProcessor

radioServer:
	$(CPP) $(CFLAGS) -o $(RSERVERTARGET) $(RSERVERFILES)

spectrumServer:
	$(CPP) $(CFLAGS) -o $(SSERVERTARGET) $(SSERVERFILES)

spectrumProcessor:
	$(CPP) $(CFLAGS) -o $(SPROCESSORTARGET) $(SPROCESSORFILES) -l fftw3

clean:
	rm $(RSERVERTARGET) $(SSERVERTARGET) $(SPROCESSORTARGET)

