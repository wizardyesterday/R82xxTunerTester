// Title: plotGainDataT.sci
// This program plots gain data when the input data file was obtained by
// using the R82xx tuner register R12 to set the IF gain of the tuner.

// mainline code.

agcData = [];

gainData = fscanfMat('myGainData.txt');

plot(gainData(:,3));
xgrid();
title('R82xx Tuner Gain vs. R82xx AGC Register Setting');
xlabel('Specifiwd Gain Value, dB');
ylabel('Gain, dB');

