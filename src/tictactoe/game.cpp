#include "game.hpp"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <stdexcept>

Player::Player(int id, std::string name):
	id(id), name(name)
{}

int Player::get_id()
{return id;}

std::string Player::get_name()
{return name;}

std::string Player::serialize() 
{
    return std::string("{")
        .append(std::to_string(id))
        .append(",")
        .append(name)
        .append("}");
}

Player *Player::parse(std::string serialized)
{
    std::vector<std::string> fields = Serializable::split_fields(serialized);
    if (fields.size() < 2)
        return nullptr;
    Player *aux = new Player(stoi(fields[0]), fields[1]);
    return aux;
}
	

Room::Room(int id):
	players(std::pair<Player*, Player*>(nullptr, nullptr)), id(id)
{}

int Room::get_id()
{
    return id;
}

bool Room::is_full()
{
    return players.first != nullptr and players.second != nullptr;
}

const int Room::EMPTY_INT_FLAG = -1;

const std::string Room::EMPTY_STR_FLAG = "\\";

std::pair<int, int> Room::get_players_id()
{
	std::pair<int, int> id;
	id.first = (players.first == nullptr) ? EMPTY_INT_FLAG : players.first->get_id();
	id.second = (players.second == nullptr) ? EMPTY_INT_FLAG : players.second->get_id();
	return id;
}

std::pair<std::string, std::string> Room::get_players_name()
{
	std::pair<std::string, std::string> names;
	names.first = (players.first == nullptr) ? EMPTY_STR_FLAG : players.first->get_name();
	names.second = (players.second == nullptr) ? EMPTY_STR_FLAG : players.second->get_name();
	return names;
}

void Room::add_player(Player *player)
{
    if (players.first == nullptr)
        players.first = player;
    else if (players.second == nullptr)
        players.second = player;
    else
        throw std::logic_error("Trying to add a player to a full room");
}

std::string Room::serialize()
{
    return std::string("{")
        .append(std::to_string(id))
        .append(",")
        .append(players.first != nullptr ? players.first->serialize() : "{}")
        .append(",")
        .append(players.second != nullptr ? players.second->serialize() : "{}")
        .append("}");
}

Room *Room::parse(std::string serialized)
{
    if (serialized.empty())
        return nullptr;
    std::vector<std::string> fields = Serializable::split_fields(serialized);
    Room *room = new Room(stoi(fields[0]));
    if (!fields[1].empty()) {
        Player *player = Player::parse(fields[1]);
        room->add_player(player);
    }
    if (!fields[2].empty()) {
        Player *player = Player::parse(fields[2]);
        room->add_player(player);
    }
    return room;
}

