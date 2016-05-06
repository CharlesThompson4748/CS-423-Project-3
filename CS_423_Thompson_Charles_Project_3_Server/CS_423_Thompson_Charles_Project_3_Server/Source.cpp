
/****************************************************
*
*	Charles Thompson
*	CS 423 - Client Server Programming
*	Project 2 - Linux Server IM Service
*	University of Missouri - Kansas City
*	School of Computing and Engineering
*	04/10/2016
*
*	This program is a simple server application for a IM (Instant Messaging) 
*	service that acts as an itermediate between users. This program uses the a UDP
*	connectionless service using the getaddrinfo() method. 
*
*	When the user starts this program the server will bind a port and use its
*	current IP for communcation. The server will then sit and wait for communication
*	from client applications and respond according to they message type recieved. If 
*	the client sends a sign on message of type 1 the server will respond with a message
*	indicating other users that are currently logged on and broadcast to other users that 
*	a new user has joined the IM service. If the user sends a forward message of type 2 
*	then a message will be sent to the client indicating the message has been recieved and
*	forward the specified message to the specified user. If the users sends a log off message
*	of type 3 then a goodbye message will be sent to the cliend and a broadcast message will 
*	be sent to all of logged on clients that the user is leaving the IM service.	
*
****************************************************/

#include "Cipher.h"

using namespace std;

#define PORT "23456"
#define BACKLOG 10

//Global Variables
int l = 0;

int main(int argc, char *argv[]) {
	//Variable decelerations
	time_t t = time(0);
	struct tm* localtm;
	filebuf logFile;
	int sockfd, newfd;
	socklen_t sin_size;
	char client_reply[500];
	string userName, client_ip, client_port, msg;
	int recv_size, return_value;
	clock_t timer;
	const char* message;
	vector<string> msgInfo;

	//Client user list
	map<string, pair<int, struct sockaddr_storage> > userList;
	map<string, pair<string, string> > usersInfo;
	pair<int, struct sockaddr_storage> client_info;

	//map<string, struct sockaddr_storage> userList;

	//Creating addinfo variables
	struct addrinfo server_struct, *servinfo, *p;
	struct sockaddr_storage client_addr;
	socklen_t addr_len;

	memset(&server_struct, 0, sizeof server_struct);
	server_struct.ai_family = AF_INET;
	server_struct.ai_socktype = SOCK_STREAM;
	server_struct.ai_flags = AI_PASSIVE;

	//Getting server address info
	if ((return_value = getaddrinfo(NULL, PORT, &server_struct, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %d\n", errno);
		return 1;
	}

	struct sockaddr_in *addr;
	addr = (struct sockaddr_in *)servinfo->ai_addr; 

	cout << "Address Resolved" << endl << inet_ntoa((struct in_addr)addr->sin_addr) << endl;

	//Create a socket	
	if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
		fprintf(stderr, "Could not create socket : %d", errno);
	}

	//set SO_RESESADDR on a socket s to true	
	int optval = 1;
	setsockopt(return_value, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof optval);
	
	//Binding socket
	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
        }

	logFile.open("logFile.txt", ios::out);
	ostream os(&logFile);
	cout << "Socket Created & Bound" << endl;
	cout << "Entering Main Loop" << endl;


	//Starting status check timer
	timer = clock();

	while (true) {
		cout << "Waiting For Communication....." << endl;
		if (timer - clock() >= 6000) {
			if(!userList.empty()) {
				cout << "In keep-alive broadcast loop" << endl;
				for(map<string, pair<int, struct sockaddr_storage> >::const_iterator it = userList.begin(); it != userList.end(); it++) {
					msg = createMessage(userName, "", "", 4, usersInfo);
					if (send(it -> second.first, message, strlen(message), 0) < 0) {
						cout << "Send Failed " << errno << endl;
						continue;
					}
				}
			}
			else {
				timer = clock();
			}
		}

		if (listen(sockfd, BACKLOG) == -1) {
			perror("listen");
			exit(1);
		}

		sin_size = sizeof client_addr;
        newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);

        if (newfd == -1) {
            perror("accept");
            continue;
        }
		client_ip = inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr*) &client_addr),client_reply, sizeof client_reply);
		printf("server: got connection from %s\n", client_reply);

		localtm = localtime(&t);
		os << asctime(localtm) <<  "Connection Recieved From ";
		os << (char *)client_ip.c_str();

		//Receive a reply from the client	
		if ((recv_size = recv(newfd, client_reply, 500, 0) == -1)) {
			cout << "recv failed" << errno << endl;
            return 1;
		}

		if (!fork()) { // this is the child process
			switch (client_reply[0]) {
				//Signon Service
				case '1':
					//Getting user ip address info and username
					msgInfo = parseMessage(client_reply);
					userName = msgInfo[1];
					client_port = msgInfo[2];
					userName = decrypt(userName, 0);
					os << " PORT: " << (char *)client_port.c_str() << " Username: " << (char *)userName.c_str();
					usersInfo.insert(pair<string, pair<string, string> >(userName, make_pair(client_ip, client_port)));
					cout << "\nClient Port = " << client_port << "\nUSERNAME = " << userName << endl;

					//Adding new user to users map
					client_info = make_pair(newfd, client_addr);
					userList.insert(pair<string, pair<int, struct sockaddr_storage> >(userName, client_info));

					//Creating reply message
					msg = createMessage(userName, "", client_port, 1, usersInfo);
					message = msg.c_str();
					//Sending message to user
					if (send(newfd, message, strlen(message), 0) < 0) {
						cout << "Send Failed " << errno << endl;
						break;
					}

					//Broadcasting signon to users
					if(userList.size() > 0) {
						msg = createMessage(userName, client_ip, client_port, 3, usersInfo);
						message = msg.c_str();
						for(map<string, pair<int, struct sockaddr_storage> >::const_iterator it = userList.begin(); it != userList.end(); it++) {
							if (send(it -> second.first, message, strlen(message), 0) < 0) {
								cout << "Send Failed " << errno << endl;
								continue;
							}
						}
					}
					break;

				//Signoff Service
				case '2':
					if(findUser(userName, userList)) {
						//Format the message to be sent 
						msg = createMessage(userName, "", client_port, 3, usersInfo);
						message = msg.c_str();
						//Send message to client for signoff
						if (send(newfd, message, strlen(message), 0) < 0) {
							cout << "Send Failed " << errno << endl;
							break;
						}
						cout << "Reply sent to user " << userName << endl;
					}
					else {
						//Failed to find user to signoff
						msg = createMessage(userName, "User Not Online", client_port, 8, usersInfo); // need to get message to be forwarded
						message = msg.c_str();
						if (send(newfd, message, strlen(message), 0) < 0) {
							cout << "Send Failed " << errno << endl;
							break;
						}
					}
					break;
				close(newfd);
			}
			cout << "Returning to top" << endl;
			client_port = " ";
			logFile.close();
		}

	}
	cout << "Closing Socket...\nServer Shutting Down" << endl;
	//Program end
	close(sockfd);
	cin.ignore().get();
	return 0;
}