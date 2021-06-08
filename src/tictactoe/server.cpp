#include "server.hpp"

#include "game.hpp"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define MESSAGE_MAX_SIZE 500

ServerConnector::ServerConnector(int socket_fd) noexcept(false):
	socket_fd(socket_fd), connected(true)
{
	client_thread = std::thread(&ServerConnector::run, this); 
}


ServerConnector::~ServerConnector() noexcept(false)
{
    if (shutdown(socket_fd, SHUT_RDWR) < 0)
        throw SocketError();
}

bool ServerConnector::is_connected()
{
	return connected;
}
        
void ServerConnector::send(GameMessage *message) noexcept(false)
{
    std::string message_str = message->serialize();
    int bytes_sent = ::send(socket_fd, message_str.c_str(), message_str.size(), 0);
    if (bytes_sent < (int)message_str.size())
	{
		connected = false;
        throw SocketError();
	}
}

GameMessage* ServerConnector::receive() noexcept(false)
{
    char buffer[MESSAGE_MAX_SIZE];
    int bytes_received = ::recv(socket_fd, buffer, MESSAGE_MAX_SIZE, 0);
    if (bytes_received < 0)
	{
		connected = false;
        throw SocketError();	
	}
    buffer[bytes_received] = '\0';
    return GameMessage::parse(std::string(buffer));
}

void ServerConnector::run() noexcept(false)
{
	GameMessage *message;
	try{
		message = this->receive();
		printf("Message received: %s", message->player_name.c_str());
		// não sei como a gnt guarda isso. acho q precisa ter um map de socket para room. 	
		//Player player(client_descriptor, message->player_name);	
	} catch (std::exception const& e) {
        std::cout << 1 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
}

GameServer::GameServer(){}

void GameServer::add_client(int client_descriptor)
{
	std::lock_guard<std::mutex> lock(client_lock);

	active_clients[client_descriptor] = std::make_shared<ServerConnector>(client_descriptor);		
}

void GameServer::delete_disconnected()
{	
	std::lock_guard<std::mutex> lock(client_lock);

	std::vector<int> to_delete;

	for(auto &[descriptor, client] : active_clients)
	{
		if(client->is_connected() == false)
		{	
			to_delete.push_back(descriptor);
		}
	}

	for (auto descriptor : to_delete) {
		// deletar no ambiente do jogo.
		printf("Deleted client %d in game server successfully!\n", descriptor);
		active_clients.erase(descriptor);
	}
}


