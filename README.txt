CPSC-471-PROJECT 1
BY: Zach Serna 		serna648@csu.fullerton.edu
    Nicole Serna	nicoleserna@csu.fullerton.edu


LANGUAGE USED: C++

HOW TO COMPILE:

1.Download the .tar file and open the tar file.

2.Open up a terminal and split the terminal into two seperate ones

3.Type "make clean" into the console

4.Type "make" into the console

5.On Console 1 type "./server (port number)
we use port number: 1234

6.On Console 2 type "./client (server IP) (port number)
we use 127.0.0.1 for the IP since we are the local host
we use port number: 1234 to match the previous port input

7.On Console 2/client Console, enter in one of the functions listed below

INTRO:
We designed a simple FTP server and client system which allows for communication
between two different systems. The server will connect to a specified port
provided by the user, after which it will wait indefinitely for a user to connect
from the client side, provided they provide the proper port as well as an IP.
Once connected, the client has one of 4 options: get, put, ls, and quit. The 
details of these operations will be explained below.


FUNCTIONS:

Get "filename": "Gets" a file from the server and downloads it to the client.

Put "filename": "Puts" a file from the client onto the server by uploading it 
onto the server.

Ls:		Lists all the files currently in the server.

Quit:		Ends the CLIENT connection to the server. The server resumes
waiting for a connection.


PROTOCOL DESIGN:

What kinds of messages will be exchanged across the control channel?

This program is designed to transfer messages designating what the client wants
the server to do, whether it be to download or upload a file.

How should the otherside respond to the messages?

The server will receive the message and return the proper response according to
the command it received. It will also output the message it received so the user 
can see it received the proper message.

What message exchanges have to take place in order to setup a file transfer channel?

The client needs to reach out with the proper port number to establish a 
connection. A handshake is then established between the client and server
and connection is confirmed.

How will the receiving side know when to start/stop receiving the file?

When the end of the file is reached, the receiving side will be notified that
they can cease download. When a command is sent to "put" a file, the 
receiving side will be notified with the appropriate command in order for it to
begin downloading.


THINGS TO TAKE NOTE OF:



