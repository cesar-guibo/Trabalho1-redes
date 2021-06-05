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

// Comunication
class ServerConnector
{
	public:
		ServerConnector(int socket):
			socket(socket), connected(true)
		{}
		
		~ServerConnector()
		{
			//update_thread.join();
			free(get_buffer);
			free(send_buffer);
			close(socket);
		}

		void update()
		{
			while(connected)
			{
				int read_size = read(socket, get_buffer, 10000);
				if(read_size > 0)
				{
					// i think we have to transform to json
				}
				else
				{
					connected = false;
					printf("Client disconnected: %d", socket);
					// client disconnected
				}
				// send state of the game.
			}
		}

		bool is_connected(){return connected;}

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
		Player(int id, std::string name):
			id(id), name(name)
		{}

		int get_id(){return id;}
	
	private:
		int id;
		std::string name;
};

class Room
{
	public:
		Room(int id):
			id(id)
		{}

		std::pair<int, int> get_players_id()
		{
			std::pair<int, int> id;
			id.first = (players.first != nullptr) ? -1 : players.first->get_id();
			id.second = (players.second != nullptr) ? -1 : players.second->get_id();
			return id;
		}

	private:
		std::pair<Player*, Player*> players;
		int id;
};

class GameServer
{
public:
	GameServer(){}
	
	void add_client(int client_descriptor)
	{
		ServerConnector *new_client;
		new_client = new ServerConnector(client_descriptor);
		active_clients[client_descriptor] = new_client;			
	}

	void delete_disconnected()
	{	
		for(auto &[descriptor, client] : active_clients)
		{
			if(client->is_connected() == false)
			{
				printf("socket to be deleted: %d", descriptor);
				if(client != nullptr){
					free(client);
					client = nullptr;
				}
				// deletar no ambiente do jogo.
				active_clients.erase(descriptor);
				printf("Deleted successfully!\n");
			}
		}
	}

	private:
	std::map<int, Room*> rooms;
	std::unordered_map<int, ServerConnector*> active_clients; // [socket_descriptor, ServerConnector]
};
