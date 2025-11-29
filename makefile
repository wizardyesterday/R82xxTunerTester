#************************************************************************
#************************************************************************

CPP = g++
CFLAGS = -O0 -g -I include

RSERVERTARGET = radioServer
SSERVERTARGET = spectrumServer
SPROCESSORTARGET = spectrumProcessor

RSERVERFILES = src/radioServer.cc src/TcpClient.cc src/IpcMessageQueue.cc 

SSERVERFILES = src/spectrumServer.cc src/IpcMessageQueue.cc 

SPROCESSORFILES = src/spectrumProcessor.cc src/SpectrumProcessor.cc

all: radioServer spectrumServer spectrumProcessor

radioServer:
	$(CPP) $(CFLAGS) -o $(RSERVERTARGET) $(RSERVERFILES)

spectrumServer:
	$(CPP) $(CFLAGS) -o $(SSERVERTARGET) $(SSERVERFILES)

spectrumProcessor:
	$(CPP) $(CFLAGS) -o $(SPROCESSORTARGET) $(SPROCESSORFILES) -l fftw3

clean:
	rm $(RSERVERTARGET) $(SSERVERTARGET) $(SPROCESSORTARGET)

