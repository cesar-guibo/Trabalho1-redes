#ifndef GAME_H
#define GAME_H

#include <string>
#include <map>
#include <utility>
#include <unordered_map>
#include <memory>
#include <optional>
#include <mutex>

#include "serializable.hpp"
#include "game_messages.hpp"
//#include "server.hpp"

class Player : Serializable
{
	public:
        Player();
		Player(int id, std::string name);
        Player(std::string serialized_player);
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
        Room(std::string serialized_room);
        int get_id();
        void add_player(Player *player) noexcept(false);
        bool is_full();
        static const int EMPTY_INT_FLAG;
        static const std::string EMPTY_STR_FLAG;
		std::pair<int, int> get_players_id();
        std::pair<std::string, std::string> get_players_name();
        std::string serialize();
        static std::shared_ptr<Room> parse(std::string serialized);
        GameMessage* get_plays();
		void set_plays(GameMessage* plays);

	private:
		std::pair<Player*, Player*> players;
		int id;
        GameMessage* plays;
        std::mutex player_lock;
};

class Board
{
    public:
        Board();
        ~Board();
        void mark_chosen(CrossOrCircle c, std::pair<int, int> position)
            noexcept(false);
        BoardState get_state();
        std::string to_string();

    private:
        std::array<std::array<char, 3>, 3> board;
        bool has_won(CrossOrCircle c);
        bool is_full();
};

#endif
