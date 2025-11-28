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
The user can type 'm' to  command the radio to set the receive IF gain, and
type 'e' which will result in the coordinator notifying both the radio server
and the spectrum server to terminate execution. Te coordinator wil also
terminate execution.
Initially, I was going to have the coordinator execute in a loop a user-
specified number of times and perform terminate processing. I decided that it
was more appropriate to provide an interactive approach so that the user
could see some sort of feedback on a per-measurment basis. The coordinator
will then autoincrement the parameters sent to the radio server and spectrum
server. It's easy enough to make the coordinator operate in a fully
automatic mode if desired. I may specify that as a command line option

it will exit.


To build this application, type 'sh buildSystem.sh'.
To remove executable files, type 'sh clean.sh'.

I'll improve this file later.

