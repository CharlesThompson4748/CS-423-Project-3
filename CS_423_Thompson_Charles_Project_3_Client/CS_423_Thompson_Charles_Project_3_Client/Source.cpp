
/****************************************************
*
*	Charles Thompson
*	CS 423 - Client Server Programming
*	University of Missouri - Kansas City
*	School of Computing and Engineering
*	03/15/2016
*
*	This program is a simple client application for a IM 
*	(Instant Messaging) service that uses a server as an 
*	itermediate between users. This program uses the a UDP
*	connectionless service using the getaddrinfo() method
*	from the winsock2 library. The program also uses 
*	multi-threading to listen for server responses for 
*	messages from other users. This is achieved using the
*	_beginthread() function from the process.h library.
*
*	When the user starts this program they will be 
*	prompted for a server and port number or the can 
*	enter 0 for the default server	information. They 
*	will then be asked to enter a user	name. The program 
*	will then establish connection to the server and 
*	indicate the other users that are connected to the IM 
*	service. The user can the enter s to send a message to 
*	another user, c to check for messages from other users, 
*	or q to quit the IM service.	
*
****************************************************/



#include "Cipher.h"

using namespace std;

//Global Variables
SOCKET s;
int l = 0;

void Listen(void * temp);

int main(int argc, char *argv[]) {
	//Default server information
	string SERVER_ADDRESS = "204.76.188.23", SERVER_PORT = "23456";

	//Variable decelerations
	WSADATA wsa;
	struct sockaddr_in server;
	char server_reply[500], userSelection;
	string userName, userBuddy, msgToSend, msg, temp = "";
	int recv_size, return_value, msgNum;
	const char* message;

	//Starting WSA
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());		
		return 1;
	}

	//Creating addinfo variables
	struct addrinfo server_struct, *servinfo, *p;

	memset(&server_struct, 0, sizeof server_struct);
	server_struct.ai_family = AF_INET;
	server_struct.ai_socktype = SOCK_DGRAM;

	//Getting server address from user
	cout << "Enter Server IP Adress or 0 for Default Adress ";
	cin >> temp;
	if (temp != SERVER_ADDRESS && temp != "0") {
		SERVER_ADDRESS = temp;
	}

	//Getting server port number from user
	cout << "Enter Server Port Number or 0 for Default Port Number ";
	cin >> temp;
	if (temp != SERVER_PORT && temp != "0") {
		SERVER_PORT = temp;
	}

	//Getting server address info
	if ((return_value = getaddrinfo(SERVER_ADDRESS.c_str(), SERVER_PORT.c_str(), &server_struct, &servinfo)) != 0) {
		printf("getaddrinfo: %s\n", GetLastError());
		return 1;
	}

	//Create a socket	
	if ((s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
	}

	//set SO_RESESADDR on a socket s to true	
	int optval = 1;
	setsockopt(return_value, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);

	//Getting ACK number for server
	msgNum = msgNumber();

	cout << "Our target server is at address " << SERVER_ADDRESS <<"port " << SERVER_PORT << "\nOur starting message number is " << 
		msgNum << "\nEnter your IM name... ";

	//Getting username
	cin >> userName;
	userName += '\n';
	//Encrypting the username
	userName = encrypt(userName);
	//Creating message to send to server
	msg = createMessage(userName, userBuddy, "", msgNum, 1);
	//Converting message to const char*
	message = msg.c_str();
	//Increasing ACK number
	msgNum++;

	//Sending message to server
	if (sendto(s, message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		cout << "Send Failed " << GetLastError() << endl;
		system("pause");
		return 1;
	}

	while (true) {
		do {
			//Receive a reply from the server	
			if ((recv_size = recvfrom(s, server_reply, 500, 0, NULL, NULL)) == SOCKET_ERROR) {
				cout << "recv failed" << GetLastError() << endl;
				system("pause");
				return 1;
			}
			//Checking for keep-alive message
			if(server_reply[3] == '4') {
				//Create keep-alive response
				msg = createMessage(userName, userBuddy, msgToSend, msgNum, 4);
				//Convert msg to const char*
				message = msg.c_str();
				//Sending message to server
				if (sendto(s, message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
					cout << "Send Failed " << GetLastError() << endl;
					system("pause");
					return 1;
				}

			}
			//Add \0 at the end of received string string before printing	
			server_reply[recv_size] = '\0';	//puts(server_reply);	
			cout << decrypt(server_reply, 10) << endl;

			//Main menu 
			cout << "Enter q (for quit), s (send msg), or c (check for msgs)";
			cin >> userSelection;

			switch (tolower(userSelection)) {
			case 's':
				//Send message to another user
				cout << "Enter Buddy Name: ";
				cin >> userBuddy;
				cout << "Enter Your Message: ";
				cin.ignore();
				getline(cin, msgToSend);
				//Creating message to send to server
				msg = createMessage(userName, userBuddy, msgToSend, msgNum, 2);
				message = msg.c_str();
				//Sending message to server
				if (sendto(s, message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
					cout << "Send Failed " << GetLastError() << endl;
					system("pause");
					return 1;
				}
				msgNum++;
				break;
			case 'c':
				//Creating child thread to listen for server reply
				_beginthread(Listen, 0, (void*)l);
				break;
			case 'q':
				//Format the message to be sent 
				msg = createMessage(userName, "", "", msgNum, 3);
				//Convert the msg string to const char*
				message = msg.c_str();
				//Send message to the server
				if (sendto(s, message, strlen(message), 0, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
					cout << "Send Failed " << GetLastError() << endl;
					system("pause");
					return 1;
				}
				break;
			}
		} while (userSelection != 'q');
		break;
	}

	//Receive a reply from the server	
	if ((recv_size = recvfrom(s, server_reply, 500, 0, NULL, NULL)) == SOCKET_ERROR) {
		cout << "recv failed" << GetLastError() << endl;
		system("pause");
		return 1;
	}

	//Add \0 at the end of received string string before printing	
	server_reply[recv_size] = '\0';	//puts(server_reply);	
	cout << decrypt(server_reply, 10) << endl;

	//Program end
	closesocket(s);
	WSACleanup();
	system("pause");
	return 0;
}

void Listen(void * temp){
	int recv_size;
	char server_reply[500];
	if ((recv_size = recvfrom(s, server_reply, 500, 0, NULL, NULL)) == SOCKET_ERROR) {
		cout << "recv failed" << GetLastError() << endl;
		system("pause");
	}
	//Add \0 at the end of received string string before printing	
	server_reply[recv_size] = '\0';	//puts(server_reply);	
	cout << decrypt(server_reply, 10) << endl;
}