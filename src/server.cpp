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
#include "tictactoe/argparse.hpp"

int incoming_descriptor, temp_descriptor;
struct sockaddr_in incoming_addr;
socklen_t addr_size;
GameServer game;	

void listen_connections();
void remove_disconnected_clients();

int main(int argc, char *argv[])
{
	ArgParser parser;
	parser.add_positional("a");
	parser.add_positional("p");
	parser.add_default("p", "12345");
	parser.add_default("a", "127.0.0.1");
	parser.parse(argc, argv);

	std::string addr = parser.get_arg("a");
	unsigned short port = stoi(parser.get_arg("p"));
    SocketAddress server_address(addr, port);
    GameMessage message;
 
	printf("Server init\n");
	if((incoming_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Failed to create socket\n");
        exit(EXIT_FAILURE);
	}
	inet_aton(addr.c_str(), (struct in_addr *) &incoming_addr.sin_addr.s_addr);
	incoming_addr.sin_family = AF_INET;
	incoming_addr.sin_port = htons(port);
	
	if((bind(incoming_descriptor, (struct sockaddr*) &incoming_addr, sizeof(incoming_addr))) < 0){
		perror("Failed in bind\n");
		exit(EXIT_FAILURE);
	}
	
	if(listen(incoming_descriptor, 4) != 0){
		perror("Failed in listen\n");
		exit(EXIT_FAILURE);
	}

	std::thread listen_connections_thread(listen_connections);
	std::thread remove_disconnected_thread(remove_disconnected_clients);

		

	
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
		game.add_client(temp_descriptor);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void remove_disconnected_clients()
{
	while(1)
	{
		game.delete_disconnected();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

