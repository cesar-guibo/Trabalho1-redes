#ifndef SERVER_H
#define SERVER_H

#include <thread>

#include "game.hpp"
#include "game_messages.hpp"
#include "socket_address.hpp"
#include "socket_error.hpp"

#include <string>
#include <mutex>
#include <memory>

class ServerConnector
{
	public:
	    ServerConnector(int socket_fd) noexcept(false);
        ~ServerConnector() noexcept(false);
		void run() noexcept(false);
		bool is_connected();
        void send(GameMessage *message) noexcept(false);
        GameMessage *receive() noexcept(false);

	private:
	    int socket_fd;
		bool connected;
		std::thread client_thread;
};

class GameServer
{
	public:
		GameServer();
		void add_client(int client_descriptor);
		void delete_disconnected();

	private:
		std::map<int, std::shared_ptr<Room>> rooms;
		std::unordered_map<int, std::shared_ptr<ServerConnector>> active_clients; // [socket_descriptor, ServerConnector]
		std::mutex client_lock;
};

#endif
