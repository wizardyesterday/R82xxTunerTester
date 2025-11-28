0.0 Introduction
This application provides the capability to perform automated testing of
rtl-sdr radios that use Rafael R82xx tuner devices, such as the R820T,
R820T2, and R828D tuners.  These tuners have an internal signal generator,
called the ring oscillator. This oscillator is supposed to be cable of
generating a set of 128 different discrete frequencies, but its VCO really
doesn't lock on all of these frequencies. On the radios that I have, 57.6MHz
seems to work reliably.
I decided to use the ring oscillator as aninternal signal source which drives
the mixer stage of the tuner. By settting the mixer gain at its maximum
value, reliable/repeatable measurements could be made.

1.0 Software Architecture
There are three applications that comprise this system.

1.1 Radio Server
This appication establishes a network connection (using TCP) to my radio diags
application that controls the radio. After this connection is established the
radio server will create a System V message queue (if it already hasn't been
already created) and block on this queue wating for commands. These commands
provide just enough data so that a text command can be constructed to send to
my radio diags. This server can also process a Terminate command for which
it will exit.

1.2 Spectrum Server
the spectrum server will create a System V message queue (if it already
hasn't been already created) and block on this queue wating for commands.
These commands provide just enough data so that a text command can be
constructed to invoke the spectrum processor application so that magnitude-
squared power and log power can be written to an output text file for
later processing (ro example, plotting gain values.).
This server can also process a Terminate command for which it will exit.

1.3 Coorsinator
This program is responsible for connecting to both, the radio server and
the spectrum server to sequentially issue commands to them. First sends a
command to the radio server and waits for an ack from the radio server using
the same message queue that was created by the radio server and the spectrum
server. Note that the coordinator creats the same message queue using the same key that the teo servers created. After sebdubg a nessage to the radio server
and waiting for an ack, the coordinator will then send a message to the
spectrum server (and wait for an ack), with the result that a power measurement
can be made. The cycle then repeats, based upon the numbe of measurements
that were specified by the user.
For manual mode, the user can type 'm' to  command the radio to set the receive
IF gain, followed by a power measurement, and type 'e' which will result in
the coordinator notifying both the radio server and the spectrum server
to terminate execution. The coordinator wil also terminate execution.
The coordinator execute in a loop a user-specified number of times and perform
it will exit.
For automatic mode, the same actions, stated above, will occur with no user
interaction. That is, the loop will be executed the user-specified number of
times, and after the loop exits, the radio server and spectrum server will
be told to terminate execution, and the coordinator will also terminate
execution.

2.0 Building the application
To build this application, type 'sh buildSystem.sh'.
To remove executable files, type 'sh clean.sh'.


