README

CS423 Project #3 Server

Developer Information:
Charles Thompson
Email: cct8v6@mail.umkc.edu
University of Missouri - Kansas City
School of Computing and Engineering

Developement Platform:
Windows 10
Visual Studio Ultimate 2012
C++

Included:
Source file(.cpp)
Cipher file(.h)
Project3server.exe
Server log file (.txt)
README
Console Screen Shots

Program Description:
This program is a simple server application for a IM (Instant Messaging) 
service that acts as an itermediate between users. This program uses the a 
TCP connection oriented service using the getaddrinfo() method. 

When the user starts this program the server will bind a port and use its
current IP for communcation. The server will then sit and wait for communication
from client applications and respond according to the message type recieved. If 
the client sends a sign on message of type 1 the server will respond with a message
indicating other users that are currently logged on and broadcast to other users that 
a new user has joined the IM service. If the user sends a port number to small or
large and error message will be sent. The server will also send error messages if
the message header is incorrect and if the userlist is full or the username is 
already in use.	

Running the Program: This program is for Linux OS machines ONLY
1. Download the zipped project file and store in a known location
2. Extract all files from the zipped file
3. Open the Linux terminal and navigate to the folder containing the project folder
4. Run the program by entering ./project3server
4a. If the project3server.exe file dosn't exist compile the file using the g++ Source.cpp -o project3server command
4b. If your system dosn't have the c++ compiler installed use the sudo apt-get g++ command to install the compiler

Using the Program:
1. Run the program using ./project3server and the server will start
2. If the server becomes unresponsive close the server/close terminal and run the .exe file again