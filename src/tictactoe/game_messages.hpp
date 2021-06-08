#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "serializable.hpp"
#include "game.hpp"
#include <string>
#include <vector>
#include <memory>

#define EMPTY -1

enum class MessageType {
    PLAYER_NAME = 0,
    AVAILABLE_ROOMS,
    SELECTED_ROOM,
    ENTERED_ROOM,
    GAME_STARTED,
    EXECUTED_PLAY,
    GAME_ENDED,
    /* When the size of the message is 0, the connection has been closes. */
    CONNECTION_CLOSED 
};

enum GameResult {
    WON, LOST, TIED
};

class GameMessage : Serializable {
    public:
        MessageType type;
        /* player_name is used for PLAYER_NAME message types. */
        std::string player_name;
        /* rooms is used for AVAILABLE_ROOMS message types. */
        std::vector<std::shared_ptr<Room>> rooms;
        /* selected_room_id is used for SELECTED_ROOM message types. */
        int selected_room_id;
        /* allowed_entry_in_room is used for ENTERED_ROOM message types */
        bool allowed_entry_in_room;
        /* Both plays first and cross_or_circle are used for GAME_STARTED
         * message types */
        bool plays_first;
        CrossOrCircle cross_or_circle;
        /* selected_coordinate is used for EXECUTED_PLAY message types */
        std::pair<int, int> selected_coordinate;
        /* result is used for GAME_ENDED message types */
        GameResult result;

        void clear();

        std::string serialize();
        static GameMessage *parse(std::string message_str);

    private:
        std::string type_to_string();
        std::string rooms_to_string();
        std::string selected_room_id_to_string();
        std::string allowed_entry_in_room_to_string();
        std::string plays_first_to_string();
        std::string cross_or_cirlce_to_string();
        std::string selected_coordinates_to_string();
        std::string result_to_string();

        static MessageType string_to_type(std::string str);
        static std::vector<std::shared_ptr<Room>> string_to_rooms(std::string str);
        static int string_to_selected_room_id(std::string str);
        static bool string_to_bool(std::string str);
        static CrossOrCircle string_to_cross_or_circle(std::string str);
        static std::pair<int, int> string_to_selected_coordinates(std::string str);
        static GameResult string_to_result(std::string str);
};

#endif
