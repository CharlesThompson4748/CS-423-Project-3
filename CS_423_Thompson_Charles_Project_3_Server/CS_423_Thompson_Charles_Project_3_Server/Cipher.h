
/****************************************************
*   File Name:  cipher_sp14.h
*   Author:     Bob Cotter
*   Date:       2/20/2014
*
*   This file provides the data arrays that provide
*   character substitution information needed to support
*   the substitution cipher used in the CS423 enhanced
*   IM project.
*
*	EDIT: This file also contains functions that we written
*	to be used for the windows UDP client side IM service.
*	These functions include encrypt() which will take a 
*	string and create and encrypted string that will be
*	returned. The decrypt() function  which takes a string 
*	and decrpypts it using the decr character array. The 
*	createMessage() function which will take information 
*	about the user, the im recipient, the message, the type 
*	of message and the user list containing the username
*	and socket and address information it will then create 
*	and message according to the type of message to be sent 
*	to the server.
*
****************************************************/



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <ctime>
#include <map>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;


//Encryption character array
char encr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 'b', '.', 0, 'c', '[', 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
'R', 'u', ',', 'q', '\t', 'Y', '\n', '\'', 'n', 's', 'v', 'e', 'H', 'o', 'N', 'M',
'r', '=', '0', ';', 'z', '/', '`', 'E', '\"', 'k', '&', '5', '>', 'i', 'p', ')',
'$', '!', '2', 'O', '(', 'I', 'J', '%', 'Z', 'g', '\\', '{', 'h', '7', 'S', 'P',
'a', ' ', 'W', 'x', 'y', 'T', '+', '8', '-', 'L', '9', 'f', '#', 'F', '\r', 'B',
'3', 'D', ']', 'V', '?', '*', 'G', '6', 'w', '@', '}', '|', 'C', 'l', '_', 'j',
'K', '^', '1', 't', 'Q', '<', 'U', 'd', 'm', ':', 'A', 'X', '\f', '4', '~', 0,
0, 0 };

//Decryption character array
char decr[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, '$', '&', 0, '|', '^', 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
'Q', 'A', '8', '\\', '@', 'G', ':', '\'', 'D', '?', 'e', 'V', '\"', 'X', '\n', '5',
'2', 'r', 'B', '`', '}', ';', 'g', 'M', 'W', 'Z', 'y', '3', 'u', '1', '<', 'd',
'i', 'z', '_', 'l', 'a', '7', ']', 'f', ',', 'E', 'F', 'p', 'Y', '/', '.', 'C',
'O', 't', ' ', 'N', 'U', 'v', 'c', 'R', '{', '%', 'H', '\r', 'J', 'b', 'q', 'n',
'6', 'P', '\t', '\f', 'w', '+', '[', 'I', 'L', '=', 'o', '9', 'm', 'x', '(', '-',
'>', '#', '0', ')', 's', '!', '*', 'h', 'S', 'T', '4', 'K', 'k', 'j', '~', 0,
0, 0 };

/*
*	This function takes a string a computes the 
*	equivalent encryption from the encr character
*	array.
*	Input: String that will be encrypted
*	Output: String that has been encrypted
*/
string encrypt(string message) {
	string encryptedMsg;
	try {
		for (int i = 0; i < message.size(); i++) {
			encryptedMsg += encr[(int)message[i]];
		}
	}
	catch (exception &e) {
		cout << "Exception thrown: " << e.what() << endl;
		exit(0);
	}
	return encryptedMsg; 
}

/*
*	This function takes a string that has been
*	encrypted and decrypts the string using the 
*	decr character array.
*	Input: String that will be decrypted
*	Output: String that has been decrypted
*/
string decrypt(string message, int length) {
	string decryptedMsg;
	try {
		for (int i = length; i < message.size(); i++) {
			decryptedMsg += decr[(int)message[i]];
		}
	}
	catch (exception &e) {
		cout << "Exception thrown: " << e.what() << endl;
		exit(0);
	}
	return decryptedMsg;
}

/*
*	Function parses client signon message
*	Input: Client String message
*	Output: Vector with client information
*/
vector<string> parseMessage(string message) {
	vector<string> msgInfo(3);
	string delimiters = ";#";
	size_t pos = 0;
	string token;
	int item = 0;
	while ((pos = message.find_first_of(delimiters)) != string::npos) {
		token = message.substr(0, pos);
		msgInfo[item] = token;
		message.erase(0, pos + delimiters.length()-1);
		item++;
	}
	cout << "Remaining Message " << message << endl;
	return msgInfo;
}


/*
*	This function takes the usernames and message as well as the message number
*	and message type and formats them for the client to recieve
*	Input: Strings for the usernames and message, integers for the message
*	number and message type
*	Output: String formatted for the client to recieve
*/
string createMessage(string userName, string message, string user_port, int messageType, map<string, pair<string, string> > users) {
	string msg = " ";
	//Signon Message
	if (messageType == 1) {
		if (users.size() > 1) {
			msg = "4;";
			msg += users.size();
			cout << "Message with count " << msg << endl;
			msg += "\n";
			for(map<string, pair<string, string> >::const_iterator it = users.begin(); it != users.end(); it++) {
				msg += encrypt(it -> first);
				msg += ";";
				msg += it -> second.first;
				msg += ";";
				msg += it -> second.second;
				msg += "\n";
			}
			msg += "#";
		}
		else {
			msg = "4;0#";
		}
		cout << "Message to send " << msg << endl;
		cout << "users size: " << users.size() << endl;
	}
	//Signoff Message
	else if (messageType == 2) {
		msg = "4;-1\n";
		msg += encrypt(userName);
		msg += ";";
		msg += message;
		msg += "#";
		cout << "Message to send " << msg << endl;
	}
	//Signon Alert
	else if(messageType == 3) {
		msg = "4;new\n"; //this will need to be updated to take send to all users
		msg += encrypt(userName);
		msg += ";";
		msg += message;
		msg += ";";
		msg += user_port;
		msg += "#";
		cout << "Message to send " << msg << endl;
	}
	//Status check
	else if (messageType == 4) {
		msg = "3;StatusCheck#";
		cout << "Message to send " << msg << endl;
	}
	//Error Alert
	else if(messageType == 5) {
		msg = "Error;";
		msg += encrypt(message);
		msg += "#";
		cout << "Message to send " << msg << endl;
	}
	//Error
	else {
		cout << "Error: Message type dosn't exist" << endl;
	}
	return msg;
}

/*
*	This function searches through a map of users for a specific username
*	and returns true if the username is found.
*	Input: Map of users and string username
*	Output: boolean 
*/
bool findUser(string userName, map <string, pair<int, struct sockaddr_storage> > users) {
	map<string, pair<int, struct sockaddr_storage> >::iterator itr;
	itr = users.find(userName);
	if(itr != users.end()) {
		return true;
	}
	return false;
}

/*
*	This function determines what AF_INET is being used for the input
*	sockaddr and return a value for the address.
*	Input: struct sockaddr
*	Output: void 
*/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
