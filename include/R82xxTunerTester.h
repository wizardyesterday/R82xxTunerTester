//**************************************************************************
// file name: R82xxTunerTester.h
//**************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This file contains all of the message defines for the R82xx tuner
// test system.
///_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __R82XXTUNERTESTER__

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Uix System V key generation support. the ftok() function uses these
// parameters for key_t generation that is used for IPC.
// Change these items to suit your application's needs.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#define KEYFILE "/home/chris/R82xxKeyFile"
#define PROJECT_ID 'A'
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Message information.
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
//_______________________________________________________
// These values allow multiplexing of different message
// types on a single queue. Each process can specify
// *which* message type to be retrieved from the message 
// queue. That's what I like about System V message queues.
//_______________________________________________________
enum MessageType
{
  RadioCmd = 1,
  RadioAck,
  SpectrumCmd,
  SpectrumAck
};
//_______________________________________________________

//_______________________________________________________
// Note that, although enumerations are being used, the
// opcodes are sent as the first parameter in the
// message text. For wxample, message payload text of
// "1 42" would imply setting of the radio IF gain of
// 42dB. The C++ source code should make this clear.
//_______________________________________________________
enum ServerOpcodes
{
  // Textual parameters: ifgain:Integer.
  SendRadioCommand = 1,

  // Textual parameters: tag:Integer, filepath:c_string.
  ExecuteApplicationProgram = 1,

  // Textual parameters: None.
  Terminate
};
//_______________________________________________________

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#endif // __R82XXTUNERTESTER__
