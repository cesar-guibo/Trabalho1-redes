#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <utility>
#include <unordered_map>

#include "server.hpp"
#include "serializable.hpp"

class Player : Serializable
{
	public:
		Player(int id, std::string name);
		int get_id();
        std::string get_name();
        std::string serialize();
        static Player *parse(std::string serialized);
	
	private:
		int id;
		std::string name;
};

class Room : Serializable
{
	public:
		Room(int id);
        void add_player(Player *player) noexcept(false);
        bool is_full();
        static const int EMPTY_INT_FLAG;
        static const std::string EMPTY_STR_FLAG;
		std::pair<int, int> get_players_id();
        std::pair<std::string, std::string> get_players_name();
        std::string serialize();
        static Room *parse(std::string serialized);
	
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
