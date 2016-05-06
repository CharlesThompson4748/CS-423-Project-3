
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
*	msgNumber() function which get a random 5 digit number 
*	to be used for the ACK number for communications between
*	the client application and the server. The createMessage()
*	function which will take information about the user, the 
*	im recipient, the message, the current ACK number, and
*	the type of message and it will then create and message 
*	according to the type of message to be sent to the server.
*
****************************************************/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <time.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Windows.h>
#include <map>
#include <vector>
#pragma comment(lib, "Ws2_32")
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
*	This function takes the usernames and message as well as the message number
*	and message type and formats them for the server to recieve
*	Input: Strings for the usernames and message, integers for the message
*	number and message type
*	Output: String formatted for the server to recieve
*/
string createMessage(string userName, string buddyName, string message, int messageType) {
	string msg = " ";
	//Signon Message
	if (messageType == 1) {
		msg = "1;";
		msg += userName + ";";
		msg += "34567#";
	}
	//Sendto Message
	else if (messageType == 2) {
		msg = "2;";
		msg += userName;
		msg += encrypt(buddyName);
		message += '\n';
		msg += encrypt(message) + "#";
	}
	//Send List of Users
	else if (messageType == 3) {
		msg = "3;";
		msg += message;
		//Followed by user count\n
	}
	//Signoff Message
	else if (messageType == 4) {
		msg = "3;";
		msg += userName;
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
vector<string> findUser(string userName, map <string, pair<string, string> > users) {
	vector<string> userInfo(3);
	map<string, pair<string, string> >::iterator itr;
	itr = users.find(userName);
	if(itr != users.end()) {
		userInfo[0] = userName;
		userInfo[1] = itr -> second.first;
		userInfo[2] = itr -> second.second;
	}
	return userInfo;
}

map <string, pair<string, string> > buildUserList(string message) {
	map <string, pair<string, string> > userList;
	string delimiters = ";#";
	size_t pos = 2;
	string token;
	vector<string>temp;
	int item = 0;
	while ((pos = message.find_first_of(delimiters)) != string::npos) {
		token = message.substr(0, pos);
		temp[item] = token;
		message.erase(0, pos + delimiters.length()-1);
		item++;
	}

	for(int i = 0; i < temp.size()-2; i+3) {
		userList.insert(pair<string, pair<string, string> >(temp[i], make_pair(temp[i+1], temp[i+2])));
	}
	cout << "Remaining Message " << message << endl;
	return userList;
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
