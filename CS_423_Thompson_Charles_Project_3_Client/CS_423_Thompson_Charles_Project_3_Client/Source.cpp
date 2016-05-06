
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
SOCKET main_socket, new_socket;
int l = 0;

void Listen(void * temp);

int main(int argc, char *argv[]) {
	//Default server information
	string SERVER_ADDRESS = "192.168.56.101", SERVER_PORT = "23456";

	//Variable decelerations
	WSADATA wsa;
	struct sockaddr_in server;
	socklen_t sin_size;
	char server_reply[500], userSelection;
	string userName, userBuddy, msgToSend, msg, temp = "", peer_ip;
	int recv_size, return_value;
	const char* message;
	vector<string> buddyInfo(3);

	map<string, pair<string, string> > userList;
	pair<string, string> client_info;

	//set of socket descriptors
    fd_set readfds;
	timeval timeout;

	//Starting WSA
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code : %d", WSAGetLastError());		
		return 1;
	}

	//Creating addinfo variables
	struct addrinfo server_struct, *servinfo, *p;

	memset(&server_struct, 0, sizeof server_struct);
	server_struct.ai_family = AF_INET;
	server_struct.ai_socktype = SOCK_STREAM;
	
	struct sockaddr_in client_addr;
	struct sockaddr_storage peer_addr;

	memset(&client_addr, 0, sizeof client_addr);
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(44567);

	struct addrinfo peer_struct, *peer_info;
	memset(&peer_struct, 0, sizeof peer_struct);
	peer_struct.ai_family = AF_INET;
	peer_struct.ai_family = SOCK_STREAM;

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

	cout << "Connecting to default server..." << endl;
	//Getting server address info
	if ((return_value = getaddrinfo(SERVER_ADDRESS.c_str(), SERVER_PORT.c_str(), &server_struct, &servinfo)) != 0) {
		printf("getaddrinfo: %s\n", GetLastError());
		return 1;
	}

	cout << "Address resloved..." << endl;
	//Create a socket	
	if ((main_socket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == INVALID_SOCKET) {
		printf("Could not create socket : %d", WSAGetLastError());
	}

	cout << "Socket Created..." << endl;

	if (bind(main_socket, (struct sockaddr*) &client_addr, sizeof client_addr) < 0) {
		perror("bind");
		exit(1);
	}

	cout << "Socket Bound..." << endl;

	//Connecting to the server
	if (connect(main_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		closesocket(main_socket);
		printf("Could not connect : %d", GetLastError());
        return 1;
	}

	cout << "Connected..." << endl;
	//set SO_RESESADDR on a socket s to true	
	int optval = 1;
	setsockopt(return_value, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);

	cout << "Our target server is at address " << SERVER_ADDRESS <<" port " << SERVER_PORT << "\nEnter your IM name... ";

	//Getting username
	cin >> userName;
	userName += '\n';
	//Encrypting the username
	userName = encrypt(userName);
	//Creating message to send to server
	msg = createMessage(userName, "", "", 1);
	//Converting message to const char*
	message = msg.c_str();


	//Sending message to server
	if (send(main_socket, message, strlen(message), 0) < 0) {
		cout << "Send Failed " << GetLastError() << endl;
		system("pause");
		return 1;
	}

	while (true) {
		 //clear the socket set
        FD_ZERO(&readfds);
        //add master socket to set
        FD_SET(main_socket, &readfds);

		timeout.tv_sec = 0;  // Zero timeout (poll)
		timeout.tv_usec = 0;
		if(select(main_socket, &readfds, NULL, NULL, &timeout) == 1)
		{
		   cout << "Bef4 socket\n";
		   main_socket = accept(main_socket , (struct sockaddr *)&peer_addr, &sin_size);  
		   cout << "Aft3r socket\n";
		}

		//Receive a reply from the server	
        if ((recv_size = recv(main_socket, server_reply, 500, 0)) == SOCKET_ERROR) {
			cout << "recv failed" << GetLastError() << endl;
			system("pause");
            return 1;
		}

        //Add \0 at the end of received string string before printing	
        server_reply[recv_size] = '\0';	//puts(server_reply);
        if(server_reply[2] != '0') {
			cout << server_reply << endl << decrypt(server_reply, 2) << endl;
        }
        else {
            cout << server_reply << " No Users Online" << endl;
        }

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
                msg = createMessage(userName, userBuddy, msgToSend, 2);
                message = msg.c_str();
				buddyInfo = findUser(userBuddy, userList);
				//Getting server address info
				if ((return_value = getaddrinfo(buddyInfo[1].c_str(), buddyInfo[2].c_str(), &peer_struct, &peer_info)) != 0) {
					printf("getaddrinfo: %s\n", GetLastError());
					return 1;
				}
				cout << "Address resloved..." << endl;

				//Create a socket	
				if ((main_socket = socket(peer_info->ai_family, peer_info->ai_socktype, peer_info->ai_protocol)) == INVALID_SOCKET) {
					printf("Could not create socket : %d", WSAGetLastError());
				}

				cout << "Socket Created..." << endl;

				if (bind(new_socket, (struct sockaddr*) &client_addr, sizeof client_addr) < 0) {
					perror("bind");
					exit(1);
				}

				cout << "Socket Bound..." << endl;

				//Connecting to the server
				if (connect(new_socket, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
					closesocket(new_socket);
					printf("Could not connect : %d", GetLastError());
					return 1;
				}
                //Sending message to server
                if (send(new_socket, message, strlen(message), 0) < 0) {
                    cout << "Send Failed " << GetLastError() << endl;
                    system("pause");
                    return 1;
                }
				closesocket(new_socket);
                break;
            case 'c':
				timeout.tv_sec = 0;  // Zero timeout (poll)
				timeout.tv_usec = 0;
				if(select(main_socket, &readfds, NULL, NULL, &timeout) == 1)
				{
					main_socket = accept(main_socket , (struct sockaddr *)&peer_addr, &sin_size); 
					//Receive a reply from the server	
					if ((recv_size = recv(main_socket, server_reply, 500, 0)) == SOCKET_ERROR) {
						cout << "recv failed" << GetLastError() << endl;
						system("pause");
						return 1;
					}
				}
                break;
            case 'q':
                cout << "Goodbye" << endl;
                break;
            default:
                cout << "Invalid Entry!" << endl;
            }
	}

	//Program end
	closesocket(main_socket);
	WSACleanup();
	system("pause");
	return 0;
}

void Listen(void * temp){
	cout << "In Listen Function..." << endl;
	int recv_size;
	char server_reply[500];
	if ((recv_size = recv(main_socket, server_reply, 500, 0)) == SOCKET_ERROR) {
		cout << "recv failed" << GetLastError() << endl;
		system("pause");
	}
	//Add \0 at the end of received string string before printing	
	server_reply[recv_size] = '\0';	//puts(server_reply);	
	cout << decrypt(server_reply, 10) << endl;
}
