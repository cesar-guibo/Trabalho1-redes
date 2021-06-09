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
#include <chrono>

#define MESSAGE_MAX_SIZE 500

#define CONNECTIONS_QUEUE_SIZE 10

ServerConnector::ServerConnector(int socket_fd, GameServer &game_server) noexcept(false):
	socket_fd(socket_fd), connected(true), game_server(game_server)
{
	client_thread = std::thread(&ServerConnector::run, this); 
}

ServerConnector::~ServerConnector() noexcept(false)
{
		client_thread.join();
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
        throw SocketError();
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
	using namespace std::chrono_literals;

	GameMessage *message;
	std::map<int, std::shared_ptr<Room>> temp_room;
	Player *player;
	int number_room;
	try{
		message = this->receive();
		std::cout << "Player name received: " << message->player_name << std::endl;
		player = new Player(socket_fd, message->player_name);
		delete message;
	} catch (std::exception const& e){
        std::cout << 1 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	try{
		message = new GameMessage();
        message->type = MessageType::AVAILABLE_ROOMS;
		temp_room = game_server.get_rooms();
		if(temp_room.empty())
		{
				message->player_name = " "; //this is just to not get an error.
		}else{
			for(auto &r: temp_room) {
				message->rooms.push_back(r.second);
            }
		}
		this->send(message);
		delete message;
	} catch (std::exception const& e){
        std::cout << e.what() << std::endl;
    }
	bool try_again = true;
	while(try_again){
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		try{
			message = this->receive();
			number_room = message->selected_room_id;
			delete message;
		} catch (std::exception const& e){
        std::cout << e.what() << std::endl;
    }
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		try{
			message = new GameMessage();
			message->type = MessageType::ENTERED_ROOM;
			temp_room = game_server.get_rooms();
			message->allowed_entry_in_room = true;			
			if(temp_room.count(number_room) == 0) // if we have to create the room
			{
				message->allowed_entry_in_room = true;
				auto new_room = std::make_shared<Room>(number_room);
				new_room->add_player(player);
				game_server.add_room(number_room, new_room);
				try_again = false;
			}else
			{
				if(temp_room[number_room]->is_full()){
					message->allowed_entry_in_room = false;
					try_again = true;
				}
				else{
					temp_room[number_room]->add_player(player);
					message->allowed_entry_in_room = true;
					try_again = false;
				}
			}
			this->send(message);
			delete message;
		}catch(std::exception const& e){
			std::cout << e.what() << std::endl;
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	
	while(1)
	{
		temp_room = game_server.get_rooms();
		if(temp_room[number_room]->is_full()) break;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	bool first;
	try{
		message = new GameMessage();
		// which one will play first.
		temp_room = game_server.get_rooms();
		message->type = MessageType::GAME_STARTED;

		if(temp_room[number_room]->get_players_id().first == socket_fd){
			message->plays_first = false;
			message->cross_or_circle = CrossOrCircle::CIRCLE;
		}else{
			message->plays_first = true;
			message->cross_or_circle = CrossOrCircle::CROSS;
		}

		first = message->plays_first;
		this->send(message);
		delete message;
	} catch (std::exception const& e){
        std::cout << e.what() << std::endl;
    }
	if(first)
	{
		message = this->receive();
		message->allowed_entry_in_room = first;
		game_server.set_plays(message, number_room);

	}
	while(1)
	{
		while(1){
			message = game_server.get_plays(number_room);
			
			if (!message) {
				std::this_thread::sleep_for(50ms);
				continue;
			}

			if (message->type == MessageType::GAME_ENDED) {
				this->send(message);
				return;
			}

			if(message->allowed_entry_in_room != first){
				break;
			}
			
			std::this_thread::sleep_for(50ms);
		}

		this->send(message);
		message = this->receive();
        message->allowed_entry_in_room = first;
		game_server.set_plays(message, number_room);
		
		if (message->type == MessageType::GAME_ENDED) {
			break;
		}
	}
}

GameServer::GameServer(){}

std::map<int, std::shared_ptr<Room>> GameServer::get_rooms()
{
	std::lock_guard<std::mutex> lock(client_lock);
	return this->rooms;
}

void GameServer::add_client(int client_descriptor)
{
	std::lock_guard<std::mutex> lock(client_lock);

	active_clients[client_descriptor] = std::make_shared<ServerConnector>(client_descriptor, *this);		
}

void GameServer::delete_disconnected()
{	
	std::lock_guard<std::mutex> lock(client_lock);

	std::vector<int> to_delete;

	for(auto const &client : active_clients)
	{
		if(client.second->is_connected() == false)
		{	
			to_delete.push_back(client.first);
		}
	}

	for (auto descriptor : to_delete) {
		// deletar no ambiente do jogo.
		printf("Deleted client %d in game server successfully!\n", descriptor);
		active_clients.erase(descriptor);
	}
}

void GameServer::add_room(int number_room, std::shared_ptr<Room> room)
{
	std::lock_guard<std::mutex> lock(client_lock);
	rooms[number_room] = room;
}

GameMessage* GameServer::get_plays(int number_room)
{
	return rooms[number_room]->get_plays();
}

void GameServer::set_plays(GameMessage* plays, int number_room)
{
	rooms[number_room]->set_plays(plays);
}

