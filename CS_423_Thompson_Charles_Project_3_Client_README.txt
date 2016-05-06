README

CS423 Project #3 Client

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
README
Console Screen Shot

Program Description:
This program is a simple client application for a IM 
(Instant Messaging) service that uses a server as an 
signon service. This program uses the a TCP connection
oriented service using the getaddrinfo() method
from the winsock2 library. The program uses the server
to recieve a list of users that are currenly logged on 
and the IP and port to use for communication with other
users.

When the user starts this program they will be 
prompted for a server and port number or the can 
enter 0 for the default server	information. They 
will then be asked to enter a user	name. The program 
will then establish connection to the server and 
indicate the other users that are connected to the IM 
service. The user can the enter s to send a message to 
another user, c to check for messages from other users, 
or q to quit the IM service.	

Running the Program:
1. Download Source file and Cipher file and save in a desired location
2. Run Visual Studio
3. Once Visual Studio is open click on FILE in upper lefthand corner
4. Navigate to and click on Open then click on File
5. Navigate to saved location of the saved files in the files explorer
6. Open the both files
7. Once the files are open click on BUILD in Visual Studio
8. Once the Source file has finished building click on Local Windows Debugger to run the program

Using the Program:
1. The program will prompt you for a IM server IP address and Port Number. You can enter your own
Server IP address or enter 0 for IP and Port number to use the default.
2. Once the server has been connected you will be asked for a Username.
3. After you have entered a usename and logged on to the server you will have three options.
You can send a message by entering 's' which will prompt you for another users name and the 
message you would like to send. You can check your messages by entering 'c' which will display
any messages from other users. Or you can exit the program by entering 'q' which will close the
connection with the IM service and exit the program.