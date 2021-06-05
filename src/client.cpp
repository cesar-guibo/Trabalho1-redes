#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "tictactoe/argparse.hpp"

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

 	int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        perror("Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(addr.c_str(), (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = htons(port);
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }
    printf("Connection was established successfully\n.");
	while(1){}

	return 0;
}
