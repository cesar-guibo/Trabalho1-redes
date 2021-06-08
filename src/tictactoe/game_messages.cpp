#include "game_messages.hpp"

#include "game.hpp"
#include <iostream>
#include <memory>

void GameMessage::clear()
{
    player_name.clear();
    rooms.clear();
}

std::string GameMessage::serialize()
{
    std::string message("{");
    message.append(type_to_string()).append(",");
    message.append(player_name).append(",");
    message.append(rooms_to_string()).append(",");
    message.append(selected_room_id_to_string()).append(",");
    message.append(allowed_entry_in_room_to_string()).append(",");
    message.append(plays_first_to_string()).append(",");
    message.append(cross_or_cirlce_to_string()).append(",");
    message.append(selected_coordinates_to_string()).append(",");
    message.append(result_to_string());
    return message.append("}");
}

GameMessage *GameMessage::parse(std::string message_str)
{
    GameMessage *message = new GameMessage();
    std::vector<std::string> fields = Serializable::split_fields(message_str);
    message->type = string_to_type(fields[0]);
    message->player_name = fields[1];
    message->rooms = string_to_rooms(fields[2]);
    message->selected_room_id = string_to_selected_room_id(fields[3]);
    message->allowed_entry_in_room = string_to_bool(fields[4]); 
    message->plays_first = string_to_bool(fields[5]); 
    message->cross_or_circle = string_to_cross_or_circle(fields[6]);
    message->string_to_selected_coordinates(fields[7]);
    message->string_to_result(fields[8]);
    return message;
}

std::string GameMessage::type_to_string()
{
    return std::to_string((int)type);
}

std::string GameMessage::rooms_to_string()
{
    std::string str("[");
    if (type == MessageType::AVAILABLE_ROOMS) {
        for (auto& room : rooms)
            str.append(room->serialize()).append(",");
    }
    if (str.back() == ',')
        str.pop_back();
    return str.append("]");
}

std::string GameMessage::selected_room_id_to_string()
{
    return type == MessageType::SELECTED_ROOM 
        ? std::to_string(selected_room_id) : "";
}

std::string GameMessage::allowed_entry_in_room_to_string()
{
    return type == MessageType::ENTERED_ROOM 
        ? std::to_string((int)allowed_entry_in_room) : "";
}

std::string GameMessage::plays_first_to_string()
{
    return type == MessageType::GAME_STARTED
            ? std::to_string((int)plays_first) : "";
}

std::string GameMessage::cross_or_cirlce_to_string()
{
    return type == MessageType::GAME_STARTED 
        ? std::to_string((int)cross_or_circle) : "";
}

std::string GameMessage::selected_coordinates_to_string()
{
    std::string str("{");
    if (type == MessageType::EXECUTED_PLAY) {
        str.append(std::to_string(selected_coordinate.first)).append(",");
        str.append(std::to_string(selected_coordinate.second));
    }
    str.append("}");
    return str;
}

std::string GameMessage::result_to_string()
{
    return type == MessageType::GAME_ENDED
            ? std::to_string((int)result) : "";
}

MessageType GameMessage::string_to_type(std::string str)
{
    return (MessageType)stoi(str);
}

std::vector<std::shared_ptr<Room>> GameMessage::string_to_rooms(std::string str)
{
    std::vector<std::string> fields = Serializable::split_fields(str);
    std::vector<std::shared_ptr<Room>> rooms;
    if (fields.size() == 1 and fields[0].empty())
        return rooms;
    for (std::string field : fields) {
        rooms.emplace_back(Room::parse(field));
    }
    return rooms;
}

int GameMessage::string_to_selected_room_id(std::string str)
{
    return !str.empty() ? stoi(str) : -1;
}

bool GameMessage::string_to_bool(std::string str)
{
    return !str.empty() ? stoi(str) : 0;
}

CrossOrCircle GameMessage::string_to_cross_or_circle(std::string str)
{
    return !str.empty() ? (CrossOrCircle)stoi(str) : (CrossOrCircle)0; 
}

std::pair<int, int> GameMessage::string_to_selected_coordinates(std::string str)
{
    std::vector<std::string> fields = Serializable::split_fields(str);
    std::pair<int, int> coordinate;
    if (fields.size() == 1 and fields[0].empty())
        return coordinate;
    coordinate.first = stoi(fields[0]);
    coordinate.second = stoi(fields[1]);
    return coordinate; 
}

GameResult GameMessage::string_to_result(std::string str)
{
    return !str.empty() ? (GameResult)stoi(str) : (GameResult)0;
}
