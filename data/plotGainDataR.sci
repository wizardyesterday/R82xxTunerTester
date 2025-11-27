// Title: plotGainDataR.sci
// This program plots gain data when the input data file was obtained by
// using the RTL2832U AGC limit registers R9 and R10 to set the IF gain
// of the tuner.

// mainline code.

agcData = [];

gainData = fscanfMat('myGainData.txt');

plot(gainData(:,3));
xgrid();
title('R82xx Tuner Gain vs. RTL2832U AGC Register Setting');
xlabel('Register Value');
ylabel('Gain, dB');

