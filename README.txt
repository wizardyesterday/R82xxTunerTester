0.0 Introduction
This application provides the capability to perform automated testing of
rtl-sdr radios that use Rafael R82xx tuner devices, such as the R820T,
R820T2, and R828D tuners.  These tuners have an internal signal generator,
called the ring oscillator. This oscillator is supposed to be cable of
generating a set of 128 different discrete frequencies, but its VCO really
doesn't lock on all of these frequencies. On the radios that I have, 57.6MHz
seems to work reliably.
I decided to use the ring oscillator as aninternal signal source which drives
the mixer stage of the tuner. By setting the mixer gain at its maximum
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

2.0 Building and Removing the Application
To build this application, type 'make servers (comments at the beginning
of the makefile provide more detail):

Type: make servers

You'll find four applications n the current directory.
1. radioServer
2. spectrumServer
3. coordinator
4. spectrumProcessor.

To remove executable files, type 'make clean'.

2.1 Running the Program
The first thing you need to do is start up my RtlSdrDiags on the computer
for which the rtl-sdr radio dongle is  connected to. localhost will work
just fine.

Next, type:./spectrumServer -B <bandwidthInHz> <fileName>

Next, type: ./radioServer -a <server IP Address> -p <server port>
             -f <frequencyInHz, optional>

Finally, type: ./coorsinator -L <start IF gain> -U <end IF gain>
               -S <start tag number> -E <end tag number>
               -A (automatic mode, optional).

For more details, run each program using the -h flag.

3. Summary
This program saved me lots of time when characterizing an rtl-sdr radio.
When performing thests using the command line, it took about 2 hours to
perform 256 measurements. When running this program in automatic mode,
the same set of measurements took 1 minute and 37 seconds.
I hope that this multi-program provides a usable example of writing
software that uses Unix System V message queues.

IRC Nick: wizardyesterday
#IRC Channels: ##rtlasr, ##trustnoone

As always, kind regards!

