#include "server.hpp"

#include "game.hpp"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

// Comunication
ServerConnector::ServerConnector(int socket):
	socket(socket), connected(true)
{
	update_thread = std::thread(&ServerConnector::update, this);
}
		
ServerConnector::~ServerConnector()
{
	update_thread.join();
	close(socket);
}

void ServerConnector::update()
{
	while(connected)
	{
		int read_size = read(socket, get_buffer, 10000);
		if(read_size > 0)
		{
			
		}
		else
		{
			connected = false;
			printf("Client disconnected: %d\n", socket);
			// client disconnected
		}
		// send state of the game.
	}
}

bool ServerConnector::is_connected(){return connected;}



// Game related

