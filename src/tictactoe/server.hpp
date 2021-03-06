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
#include <optional>

class ServerConnector;

class GameServer
{
	public:
		GameServer();
		void add_client(int client_descriptor);
		void delete_disconnected();
		std::map<int, std::shared_ptr<Room>> get_rooms();
		void add_room(int number_room, std::shared_ptr<Room> room);
		void delete_room(int number_room);
		GameMessage* get_plays(int number_room);
		void set_plays(GameMessage* plays, int number_room);

	private:
		std::map<int, std::shared_ptr<Room>> rooms;
		std::unordered_map<int, std::shared_ptr<ServerConnector>> active_clients; // [socket_descriptor, ServerConnector]
		std::mutex client_lock;
};

class ServerConnector
{
public:
	ServerConnector(int socket_fd, GameServer &game_server) noexcept(false);
    ~ServerConnector() noexcept(false);
	void run() noexcept(false);
	bool is_connected();
    void send(GameMessage *message) noexcept(false);
    GameMessage *receive() noexcept(false);

private:
	int socket_fd;
	bool connected;
	GameServer &game_server;
	std::thread client_thread;
	std::mutex game_lock;
};

#endif
