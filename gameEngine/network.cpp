/* ***********************************************************************************
 * Copyright (C) 2015-2018 Xavier Direz - http://www.LaFaceObscureDuPixel.fr         *
 *                                                                                   *
 * This file is part of ObscureGameEngine.                                           *
 *                                                                                   *
 * ObscureGameEngine is free software; you can redistribute it and/or modify         *
 * it under the terms of the GNU Lesser General Public License as published by       *
 * the Free Software Foundation; either version 3 of the License, or                 *
 * (at your option) any later version.                                               *
 *                                                                                   *
 * ObscureGameEngine is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                    *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                     *
 * GNU Lesser General Public License for more details.                               *
 *                                                                                   *
 * You should have received a copy of the GNU Lesser General Public License          *
 * along with  this program; If not, see <http://www.gnu.org/licenses/>.             *
 *************************************************************************************/
 
#include "network.hpp"

// class NetworkServer

NetworkServer::NetworkServer(unsigned short _PORT, unsigned short _BUFFER_SIZE, unsigned short _MAX_CLIENTS, Engine* _engine)
	:PORT(_PORT), BUFFER_SIZE(_BUFFER_SIZE), MAX_CLIENTS(_MAX_CLIENTS), MAX_SOCKETS(_MAX_CLIENTS + 1), engine(_engine)
{
	clientSocket = new TCPsocket[MAX_CLIENTS];
	socketIsFree = new bool[MAX_CLIENTS];
	buffer = new char[BUFFER_SIZE];
	receivedByteCount = 0;
	clientCount = 0;
}

NetworkServer::~NetworkServer()
{
	// Free our socket set (i.e. all the clients in our socket set)
	SDLNet_FreeSocketSet(socketSet);

	// Close our server socket, cleanup SDL_net and finish!
	SDLNet_TCP_Close(serverSocket);

	delete [] clientSocket;
	delete [] socketIsFree;
	delete [] buffer;
}

bool NetworkServer::init()
{
	// Create the socket set with enough space to store our desired number of connections (i.e. sockets)
	socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
	if (socketSet == NULL)
	{
		cout << "Failed to allocate the socket set: " << SDLNet_GetError() << "\n";
		return false; // Quit!
	}
	else
	{
		cout << "Allocated socket set with size:  " << MAX_SOCKETS << ", of which " << MAX_CLIENTS << " are availble for use by clients." <<  endl;
	}

	// Initialize all the client sockets (i.e. blank them ready for use!)
	for (int loop = 0; loop < MAX_CLIENTS; loop++)
	{
		clientSocket[loop] = NULL;
		socketIsFree[loop] = true; // Set all our sockets to be free (i.e. available for use for new client connections)
	}

	// Try to resolve the provided server hostname. If successful, this places the connection details in the serverIP object and creates a listening port on the provided port number
	// Note: Passing the second parameter as "NULL" means "make a listening port". SDLNet_ResolveHost returns one of two values: -1 if resolving failed, and 0 if resolving was successful
	int hostResolved = SDLNet_ResolveHost(&serverIP, NULL, PORT);

	if (hostResolved == -1)
	{
		cout << "Failed to resolve the server host: " << SDLNet_GetError() << endl;
	}
	else // If we resolved the host successfully, output the details
	{
		// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
		Uint8 * dotQuad = (Uint8*)&serverIP.host;

		//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
		cout << "Successfully resolved server host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
		cout << " port " << SDLNet_Read16(&serverIP.port) << endl << endl;
	}

	// Try to open the server socket
	serverSocket = SDLNet_TCP_Open(&serverIP);

	if (!serverSocket)
	{
		cout << "Failed to open the server socket: " << SDLNet_GetError() << "\n";
		return false;
	}
	else
	{
		cout << "Sucessfully created server socket." << endl;
	}

	// Add our server socket to the socket set
	SDLNet_TCP_AddSocket(socketSet, serverSocket);

	cout << "Awaiting clients..." << endl;
	return true;
}

void NetworkServer::acceptConnections()
{
	// Check if our server socket has received any data
	// Note: SocketReady can only be called on a socket which is part of a set and that has CheckSockets called on it (the set, that is)
	// SDLNet_SocketReady returns non-zero for activity, and zero is returned for no activity. Which is a bit bass-ackwards IMHO, but there you go.
	int serverSocketActivity = SDLNet_SocketReady(serverSocket);

	// If there is activity on our server socket (i.e. a client has trasmitted data to us) then...
	if (serverSocketActivity != 0)
	{
		// If we have room for more clients...
		if (clientCount < MAX_CLIENTS)
		{

			// Find the first free socket in our array of client sockets
			int freeSpot = -99;
			for (int loop = 0; loop < MAX_CLIENTS; loop++)
			{
				if (socketIsFree[loop] == true)
				{
					//cout << "Found a free spot at element: " << loop << endl;
					socketIsFree[loop] = false; // Set the socket to be taken
					freeSpot = loop;            // Keep the location to add our connection at that index in the array of client sockets
					break;                      // Break out of the loop straight away
				}
			}

			// ...accept the client connection and then...
			clientSocket[freeSpot] = SDLNet_TCP_Accept(serverSocket);

			// ...add the new client socket to the socket set (i.e. the list of sockets we check for activity)
			SDLNet_TCP_AddSocket(socketSet, clientSocket[freeSpot]);

			// Increase our client count
			clientCount++;

			// Send a message to the client saying "OK" to indicate the incoming connection has been accepted
			strcpy( buffer, SERVER_NOT_FULL );
			int msgLength = strlen(buffer) + 1;
			SDLNet_TCP_Send(clientSocket[freeSpot], (void *)buffer, msgLength);

			cout << "Client connected. There are now " << clientCount << " client(s) connected." << endl << endl;
		}
		else // If we don't have room for new clients...
		{
			cout << "*** Maximum client count reached - rejecting client connection ***" << endl;

			// Accept the client connection to clear it from the incoming connections list
			TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);

			// Send a message to the client saying "FULL" to tell the client to go away
			strcpy( buffer, SERVER_FULL );
			int msgLength = strlen(buffer) + 1;
			SDLNet_TCP_Send(tempSock, (void *)buffer, msgLength);

			// Shutdown, disconnect, and close the socket to the client
			SDLNet_TCP_Close(tempSock);
		}

	}
}

void NetworkServer::update()
{
    // Check for activity on the entire socket set. The second parameter is the number of milliseconds to wait for.
    // For the wait-time, 0 means do not wait (high CPU!), -1 means wait for up to 49 days (no, really), and any other number is a number of milliseconds, i.e. 5000 means wait for 5 seconds
    int numActiveSockets = SDLNet_CheckSockets(socketSet, 0);

    if (numActiveSockets != 0)
    {
        cout << "There are currently " << numActiveSockets << " socket(s) with data to be processed." << endl;
    }
	this->acceptConnections();
	this->getMessages();
}

void NetworkServer::getMessages()
{
	// Loop to check all possible client sockets for activity
	for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)
	{
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		int clientSocketActivity = SDLNet_SocketReady(clientSocket[clientNumber]);

		//cout << "Just checked client number " << clientNumber << " and received activity status is: " << clientSocketActivity << endl;

		// If there is any activity on the client socket...
		if (clientSocketActivity != 0)
		{
			// Check if the client socket has transmitted any data by reading from the socket and placing it in the buffer character array
			receivedByteCount = SDLNet_TCP_Recv(clientSocket[clientNumber], buffer, BUFFER_SIZE);

			// If there's activity, but we didn't read anything from the client socket, then the client has disconnected...
			if (receivedByteCount <= 0)
			{
				//...so output a suitable message and then...
				cout << "Client " << clientNumber << " disconnected." << endl << endl;

				//... remove the socket from the socket set, then close and reset the socket ready for re-use and finally...
				SDLNet_TCP_DelSocket(socketSet, clientSocket[clientNumber]);
				SDLNet_TCP_Close(clientSocket[clientNumber]);
				clientSocket[clientNumber] = NULL;

				// ...free up their slot so it can be reused...
				socketIsFree[clientNumber] = true;

				// ...and decrement the count of connected clients.
				clientCount--;

				cout << "Server is now connected to: " << clientCount << " client(s)." << endl << endl;
			}
			else // If we read some data from the client socket...
			{
				// Output the message the server received to the screen
				cout << "Received: >>>> " << buffer << " from client number: " << clientNumber << endl;

				this->messageProcessing(clientNumber, string(buffer));

			}

		} // End of if client socket is active check

	} // End of server socket check sockets loop
}

void NetworkServer::messageProcessing(int clientNumber, string message)
{

}

// class NetworkClient

NetworkClient::NetworkClient(unsigned short _PORT, unsigned short _BUFFER_SIZE, string _serverName, Engine* _engine)
	:PORT(_PORT), BUFFER_SIZE(_BUFFER_SIZE), serverName(_serverName), engine(_engine)
{
    buffer = new char[BUFFER_SIZE];
}

NetworkClient::~NetworkClient()
{
	// Close our socket, cleanup SDL_net, reset the terminal mode and finish!
	SDLNet_TCP_Close(clientSocket);
	delete [] buffer;
}

bool NetworkClient::init()
{
	// Create the socket set with enough space to store our desired number of connections (i.e. sockets)
	socketSet = SDLNet_AllocSocketSet(1);
	if (socketSet == NULL)
	{
		cout << "Failed to allocate the socket set: " << SDLNet_GetError() << "\n";
		return false; // Quit!
	}
	else
	{
		cout << "Successfully allocated socket set." << endl;
	}

	// Try to resolve the host. If successful, this places the connection details in the serverIP object
	int hostResolved = SDLNet_ResolveHost(&serverIP, serverName.c_str(), PORT);

	if (hostResolved == -1)
	{
		cout << "Failed to resolve the server hostname: " << SDLNet_GetError() << "\nContinuing...\n";
	}
	else // If we successfully resolved the host then output the details
	{
		// Get our IP address in proper dot-quad format by breaking up the 32-bit unsigned host address and splitting it into an array of four 8-bit unsigned numbers...
		Uint8 * dotQuad = (Uint8*)&serverIP.host;

		//... and then outputting them cast to integers. Then read the last 16 bits of the serverIP object to get the port number
		cout << "Successfully resolved host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[2] << "." << (unsigned short)dotQuad[3];
		cout << " port " << SDLNet_Read16(&serverIP.port) << endl << endl;
	}

	// Try to resolve the IP of the server, just for kicks
	if ((host = SDLNet_ResolveIP(&serverIP)) == NULL)
	{
		cout << "Failed to resolve the server IP address: " << SDLNet_GetError() << endl;
		return false;
	}
	else
	{
		cout << "Successfully resolved IP to host: " << host << endl;
	}

	// Try to open a connection to the server and quit out if we can't connect
	clientSocket = SDLNet_TCP_Open(&serverIP);
	if (!clientSocket)
	{
		cout << "Failed to open socket to server: " << SDLNet_GetError() << "\n";
		return false;
	}
	else // If we successfully opened a connection then check for the server response to our connection
	{
		cout << "Connection okay, about to read connection status from the server..." << endl;

		// Add our socket to the socket set for polling
		SDLNet_TCP_AddSocket(socketSet, clientSocket);

		// Wait for up to five seconds for a response from the server
		// Note: If we don't check the socket set and WAIT for the response, we'll be checking before the server can respond, and it'll look as if the server sent us nothing back
		int activeSockets = SDLNet_CheckSockets(socketSet, 5000);

		cout << "There are " << activeSockets << " socket(s) with data on them at the moment." << endl;

		// Check if we got a response from the server
		int gotServerResponse = SDLNet_SocketReady(clientSocket);

		if (gotServerResponse != 0)
		{
			cout << "Got a response from the server... " << endl;
			int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);

			cout << "Got the following from server: " << buffer << "(" << serverResponseByteCount << " bytes)" << endl;

			// We got an okay from the server, so we can join!
			if ( strcmp(buffer, SERVER_NOT_FULL) == 0 )
			{
				cout << "Joining server now..." << endl << endl;
			}
			else
			{
				cout << "Server is full... Terminating connection." << endl;
				return false;
			}
		}
		else
		{
			cout << "No response from server..." << endl;
			return false;
		}

	}

	return true;
}

string NetworkClient::sendMessage(string message)
{
	//cout << "user pressed return" << endl;

	// Copy our user's string into our char array called "buffer"
	strcpy( buffer, message.substr(0,BUFFER_SIZE).c_str() );

	// Calculate the length of our input and then add 1 (for the terminating character) to get the total number of characters we need to send
	int inputLength = strlen(buffer) + 1;

	// Send the message to the server
	if (SDLNet_TCP_Send(clientSocket, (void *)buffer, inputLength) < inputLength)
	{
	    stringstream ss;

		ss << "Failed to send message: " << SDLNet_GetError() << endl;
		return ss.str();
	}
	else
	{
		//cout << "Message sent successfully." << endl;
	}
	return "OK";
}

string NetworkClient::getMessage(int _wait)
{
	// Check our socket set for activity. Don't wait if there's nothing on the socket just continue
	int socketActive = SDLNet_CheckSockets(socketSet, _wait);
	string message = "";
	//cout << "Sockets with data on them at the moment: " << activeSockets << endl;

	if (socketActive != 0)
	{
		// Check if we got a response from the server
		int messageFromServer = SDLNet_SocketReady(clientSocket);

		if (messageFromServer != 0)
		{
			//cout << "Got a response from the server... " << endl;
			int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
			buffer[serverResponseByteCount]='\0';
			int nb_iterations = 0;
            while(serverResponseByteCount>0)
            {
                nb_iterations++;
                //cout << "Received: " << buffer << endl;// "(" << serverResponseByteCount << " bytes)" << endl;

                message += string(buffer);
                cout << "Longueur du message : " << message.size() << endl;

                serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
                buffer[serverResponseByteCount]='\0';
            }
            messageProcessing(message);

            cout << endl << "Nombre d'iterations : " << nb_iterations << endl << endl;

            if(nb_iterations==0)
            {
                cout << "Serveur injoignable" << endl;
                // TODO : throwing exception
            }
		}
		else
		{
			// Pas de message venant du serveur
		}

	}

	cout << message << endl;
	message = message.substr(message.find("\r\n\r\n")+4);
	return message;
}

void NetworkClient::messageProcessing(const string &message)
{
}
