#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <utility>
#include <unordered_map>

#include "server.hpp"

class Player
{
	public:
		Player(int id, std::string name);
		int get_id();
	
	private:
		int id;
		std::string name;
};

class Room
{
	public:
		Room(int id);
		std::pair<int, int> get_players_id();
	
	private:
		std::pair<Player*, Player*> players;
		int id;
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
