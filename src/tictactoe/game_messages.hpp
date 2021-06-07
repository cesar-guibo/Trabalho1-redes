#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "serializable.hpp"
#include "game.hpp"
#include <string>
#include <vector>

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

enum class CorssOrCircle {
    CROSS, CIRCLE
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
        std::vector<Room*> rooms;
        /* selected_room_id is used for SELECTED_ROOM message types. */
        int selected_room_id;
        /* allowed_entry_in_room is used for ENTERED_ROOM message types */
        bool allowed_entry_in_room;
        /* Both plays first and cross_or_circle are used for GAME_STARTED
         * message types */
        bool plays_first;
        CorssOrCircle cross_or_circle;
        /* selected_coordinate is used for EXECUTED_PLAY message types */
        std::pair<int, int> selected_coordinate;
        /* result is used for GAME_ENDED message types */
        GameResult result;

        std::string serialize();
        static GameMessage *parse(std::string message_str);
};

#endif
