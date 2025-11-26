// Title: plotGainData.sci

// mainline code.

agcData = [];

gainData = fscanfMat('myGainData.txt');

plot(gainData(:,3));
xgrid();
title('R82xx Tuner Gain vs. RTL2832U AGC Register Setting');
xlabel('Register Value');
ylabel('Gain, dB');

