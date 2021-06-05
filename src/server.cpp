#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <thread>

#include "tictactoe/game.hpp"
#include "tictactoe/server.hpp"

int incoming_descriptor, temp_descriptor;
struct sockaddr_in incoming_addr;
socklen_t addr_size;
GameServer server;	

void listen_connections();
void remove_disconnected_clients();

int main(int argc, char *argv[])
{
	printf("Server init\n");
	if((incoming_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failed to create socket\n");
        exit(EXIT_FAILURE);
	}
	inet_aton(argv[1], (struct in_addr *) &incoming_addr.sin_addr.s_addr);
	incoming_addr.sin_family = AF_INET;
	incoming_addr.sin_port = htons((unsigned short) atoi(argv[2]));
	
	if((bind(incoming_descriptor, (struct sockaddr*) &incoming_addr, sizeof(incoming_addr))) < 0){
		perror("Failed in bind\n");
		exit(EXIT_FAILURE);
	}
	
	if(listen(incoming_descriptor, 4) != 0){
		perror("Failed in listen\n");
		exit(EXIT_FAILURE);
	}

	// thread for connection (new clients)
	//listen_connections();
	std::thread listen_connections_thread(listen_connections);
	std::thread remove_disconnected_thread(remove_disconnected_clients);
	// logic of the game
	
	listen_connections_thread.join();
	remove_disconnected_thread.join();

	return 0;
}
 
void listen_connections()
{
	while(1)
	{
		addr_size = sizeof(incoming_addr);
		temp_descriptor = accept(incoming_descriptor, (struct sockaddr*) &incoming_addr, &addr_size);
		printf("Accepted connection at descriptor %d\n", temp_descriptor);
		server.add_client(temp_descriptor);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	// thread sleep	
	}
}

void remove_disconnected_clients()
{
	while(1)
	{
		server.delete_disconnected();
		// disconnect in the game.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}
}

