#!/bin/sh

echo Building clients
sh buildClients.sh

echo building servers
sh buildServers.sh

echo building Spectrum Processor
sh buildSpectrumProcessor.sh

echo Build complete!

