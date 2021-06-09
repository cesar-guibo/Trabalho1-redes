#include "game.hpp"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <optional>

Player::Player(){}
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
	id.first = (players.first == nullptr) 
        ? EMPTY_INT_FLAG : players.first->get_id();
	id.second = (players.second == nullptr)
        ? EMPTY_INT_FLAG : players.second->get_id();
	return id;
}

std::pair<std::string, std::string> Room::get_players_name()
{
	std::pair<std::string, std::string> names;
	names.first = (players.first == nullptr)
        ? EMPTY_STR_FLAG : players.first->get_name();
	names.second = (players.second == nullptr)
        ? EMPTY_STR_FLAG : players.second->get_name();
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

std::shared_ptr<Room> Room::parse(std::string serialized)
{
    if (serialized.empty())
        return nullptr;
    std::vector<std::string> fields = Serializable::split_fields(serialized);
    auto room = std::make_shared<Room>(stoi(fields[0]));
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

Board::Board()
{
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
}

Board::~Board() {}

void Board::mark_chosen(CrossOrCircle c, std::pair<int, int> position)
noexcept(false)
{
    if (position.first > 3 or position.second > 3)
        throw std::invalid_argument("Position is invalid");
    if (board[position.first][position.second] != ' ')
        throw std::invalid_argument("Position is already marked");
    if (c == CrossOrCircle::CROSS)
        board[position.first][position.second] = 'X';
    else 
        board[position.first][position.second] = 'O';
}

bool Board::has_won(CrossOrCircle c)
{
    char ch = c == CrossOrCircle::CROSS ? 'X' : 'O';
    bool some_column = false;
    bool some_line = false;
    bool main_diagonal = true;
    bool secondary_diagonal = true;
    for (int i = 0; i < 3; i++) {
        main_diagonal = main_diagonal and board[i][i] == ch;
        secondary_diagonal = secondary_diagonal and board[i][3-i] == ch;
        bool column = true;
        bool line = true;
        for (int j = 0; j < 3; j++) {
            column = column and board[i][j] == ch;
            line = line and board[j][i] == ch;
        }
        some_column = some_column or column;
        some_line = some_line or line;
    }
    return some_column or some_line or main_diagonal or secondary_diagonal;
}

bool Board::is_full()
{
    bool is_full = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            is_full = is_full and board[i][j] != ' ';
        }
    }
    return is_full;
}

BoardState Board::get_state()
{
    if (has_won(CrossOrCircle::CROSS))
        return BoardState::CROSS_WON;
    if (has_won(CrossOrCircle::CIRCLE)) 
        return BoardState::CIRCLE_WON;
    if (is_full())
        return BoardState::TIED;
    return BoardState::NOT_FINISHED;
}

std::string Board::to_string()
{
    std::string str("  |   |  \n--+---+--\n  |   |  \n--+---+--\n  |   |  \n");
    int str_index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            str[str_index + 4 * j] = board[i][j];
        }
        str_index += 20;
    }
    return str;
}

GameMessage* Room::get_plays()
{
    std::lock_guard<std::mutex> lock(player_lock);
	return plays;
}

void Room::set_plays(GameMessage* plays)
{
    std::lock_guard<std::mutex> lock(player_lock);
	this->plays = plays;
}
