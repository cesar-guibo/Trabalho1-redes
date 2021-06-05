#include "game.hpp"

Player::Player(int id, std::string name):
	id(id), name(name)
{}

int Player::get_id()
{return id;}
	

Room::Room(int id):
	id(id)
{}

std::pair<int, int> Room::get_players_id()
{
	std::pair<int, int> id;
	id.first = (players.first != nullptr) ? -1 : players.first->get_id();
	id.second = (players.second != nullptr) ? -1 : players.second->get_id();
	return id;
}

GameServer::GameServer(){}

void GameServer::add_client(int client_descriptor)
{
	ServerConnector *new_client;
	new_client = new ServerConnector(client_descriptor);
	active_clients[client_descriptor] = new_client;			
}

void GameServer::delete_disconnected()
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
