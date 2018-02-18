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
 
#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <SDL2/SDL_net.h>
#include <iostream>
#include <string.h>
#include <string>

#include "engine.hpp"

using namespace std;

// Messages to send back to any connecting client to let them know if we can accept the connection or not
#define SERVER_NOT_FULL "OK"
#define SERVER_FULL "FULL"

class NetworkServer
{
	public:

    Engine* engine;

	unsigned short PORT;
	unsigned short BUFFER_SIZE;
	unsigned short MAX_SOCKETS;
	unsigned short MAX_CLIENTS;
	IPaddress serverIP;                  // The IP of the server (this will end up being 0.0.0.0 - which means roughly "any IP address")
	TCPsocket serverSocket;              // The server socket that clients will use to connect to us
	TCPsocket *clientSocket; // An array of sockets for the clients, we don't include the server socket (it's specified separately in the line above)
	bool* socketIsFree; // An array of flags to keep track of which client sockets are free (so we know whether we can use the socket for a new client connection or not)
	SDLNet_SocketSet socketSet;
	int clientCount;
	char* buffer;					// Array of characters used to store the messages we receive
	int receivedByteCount;		// A variable to keep track of how many bytes (i.e. characters) we need to read for any given incoming message i.e. the size of the incoming data

	NetworkServer(unsigned short _PORT, unsigned short _BUFFER_SIZE, unsigned short _MAX_CLIENTS, Engine* _engine);
	~NetworkServer();
	bool init();
	void update();
	void acceptConnections();
	void getMessages();
	virtual void messageProcessing(int senderRank, string message);
};

class NetworkClient
{
	public:

    Engine* engine;

	const char *host;         // Where we store the host name
	unsigned short PORT;
	IPaddress serverIP;       // The IP we will connect to
	TCPsocket clientSocket;   // The socket to use
	string    serverName;     // The server name
	char *buffer; // Array of character's we'll use to transmit our message. We get input into the userInput string for ease of use, then just copy it to this character array and send it.
	unsigned short BUFFER_SIZE;
	SDLNet_SocketSet socketSet;

	NetworkClient(unsigned short _PORT, unsigned short _BUFFER_SIZE, string _serverName, Engine* _engine);
	~NetworkClient();
	bool init();
	string getMessage(int _wait = 0);
	string sendMessage(string message);
	virtual void messageProcessing(const string &message);
};

#endif
