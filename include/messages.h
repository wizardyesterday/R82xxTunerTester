//**************************************************************************
// file name: messages.h
//**************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This file contains all of the message defines for the R82xx tuner
// test system.
///_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __MESSAGES__

enum MessageType
{
  RadioCmd = 1,
  RadioAck,
  SpectrumCmd,
  SpectrumAck
};

enum RadioServerOpcode
{
  SendRadioCommand = 1;
  Terminate
};

enum SpectrumServerOpcode
{
  ExecuteProgram = 1;
  Terminate
};

#endif __MESSAGES__
