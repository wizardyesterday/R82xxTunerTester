This directory contains power versus mangled RTL2832U register settings for the
lower and upper limits for the analog AGC. These register values determine the input to
the sigma-delta D/A that drives the analog IF gain control line that drives the IF
gain input to the R82xx tuner device, through a lowpass filter. The lowpass filter
output is a dc value that actually drives the IF gain input to the tuner device.

The tuner parameters are as follows:
1. Mixer fain: 15dB (approx.)
2. IF gain variable
3. Ring oscillator frequency: 57.6MHz
4. Ring oscillator output attenuation: 0dB (max output level)

The power measurment was computed over a bandwidth of +-500Hz about center frequency.

The file has the following formate for each record:

<IF gain, just a tag> <magnitude-squared representation of power> <log power>


