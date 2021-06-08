#ifndef SERVER_H
#define SERVER_H

#include <thread>

#include "game.hpp"
#include "game_messages.hpp"
#include "socket_address.hpp"
#include "socket_error.hpp"
#include <string>

class ServerConnector
{
	public:
	    ServerConnector(int socket_fd) noexcept(false);
        ~ServerConnector() noexcept(false);
		bool is_connected();
        void send(GameMessage *message) noexcept(false);
        GameMessage *receive() noexcept(false);

	private:
	    int socket_fd;
		bool connected;
};

class GameServer
{
	public:
		GameServer();
		void add_client(int client_descriptor);
		void delete_disconnected();

	private:
		std::map<int, Room*> rooms;
		std::unordered_map<int, ServerConnector*> active_clients; // [socket_descriptor, ServerConnector]
};

#endif
