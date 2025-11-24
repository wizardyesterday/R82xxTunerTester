//**************************************************************************
// file name: R82xxTunerTester.h
//**************************************************************************
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// This file contains all of the message defines for the R82xx tuner
// test system.
///_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __R82XXTUNERTESTER__

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// Unix System V key generation support. The ftok() function uses these
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
// types in a single queue. Each process can specify
// *which* message type to be retrieved from the message 
// queue. That's what I like about System V message
// queues.
//_______________________________________________________
enum MessageType
{
  RadioServerTypeCommand = 1,
  RadioServerTypeAck,
  SpectrumTypeServerCmd,
  SpectrumServerTypeAck
};
//_______________________________________________________

//_______________________________________________________
// These enumerations are available for all servers. A
// specific server will use enumerations that are
// specific to that particular server. The first part 
// of the enumeration identifies the server and the
// rest of the enumeration describes the command
// that is to be executed by the server. The Terminate
// enumeration applies to all servers. It tells a
// server to terminate execution.
// The C++ source code should make this clearer.
// Thus, we have a two-lwvwl hierarchy of a server
// command: MessageType:Command. The message type is
// like a routing code to a particular server, and the
// command tells what the server is to do.
// This will become clearer upon examination of the
// opcode decoder in the source code.
//_______________________________________________________
enum ServerCommands
{
  // Textual parameters: None.
  CmdTerminate,

  // Textual parameters: ifgain:Integer.
  RadioServerCmdSetIfGain,

  // Textual parameters: tag:Integer,
  SpectrumServerCmdComputePower
};
//_______________________________________________________

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#endif // __R82XXTUNERTESTER__

