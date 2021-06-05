#ifndef _GAME_SERVER_H
#define _GAME_SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <unordered_map>

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

class ServerConnector
{
	public:
		ServerConnector(int socket);
		~ServerConnector();
		void update();
		bool is_connected();

	private:
		int socket;
		char *get_buffer;
		char *send_buffer;
		bool connected;
};


// Game related

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
