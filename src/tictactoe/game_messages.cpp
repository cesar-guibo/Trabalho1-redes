#include "game_messages.hpp"

std::string GameMessage::serialize()
{
    std::string message("{");
    message.append(std::to_string((int)type));
    message.append(",");
    message.append(type == MessageType::PLAYER_NAME ? player_name : "");
    message.append(",");
    if (type == MessageType::AVAILABLE_ROOMS) {
        message.append("[");
        for (Room *room : rooms)
            message.append(room->serialize()).append(",");
        message.pop_back();
        message.append("]");
    }
    message.append(",");
    message.append(type == MessageType::SELECTED_ROOM
            ? std::to_string(selected_room_id) : "");
    message.append(",");
    message.append(type == MessageType::ENTERED_ROOM 
            ? std::to_string((int)allowed_entry_in_room) : "");
    message.append(",");
    message.append(type == MessageType::GAME_STARTED
            ? std::to_string((int)plays_first) : "");
    message.append(",");
    message.append(type == MessageType::GAME_STARTED
            ? std::to_string((int)cross_or_circle) : "");
    message.append(",");
    if (type == MessageType::EXECUTED_PLAY) {
        message.append("{").append(std::to_string(selected_coordinate.first))
            .append(",").append(std::to_string(selected_coordinate.second))
            .append("}");
    }
    message.append(",");
    message.append(type == MessageType::GAME_ENDED
            ? std::to_string((int)result) : "");
    return message.append("}");
}

GameMessage *GameMessage::parse(std::string message_str)
{
    GameMessage *message = new GameMessage();
    message_str = message_str.substr(1, message_str.size() - 2);
    std::vector<std::string> fields = Serializable::split_fields(message_str);
    message->type = (MessageType)stoi(fields[0]);
    message->player_name = !fields[1].empty()
        ? std::string(fields[1]) : std::string("");
    message->rooms.clear();
    if (!fields[2].empty()) {
        fields[2] = fields[2].substr(1, fields[2].size() - 2);
        std::vector<std::string> subfields = Serializable::split_fields(fields[2]);
        for (std::string subfield : subfields) {
            message->rooms.push_back(Room::parse(subfield));
        }
    }
    message->selected_room_id = !fields[3].empty() ? stoi(fields[3]) : -1;
    message->allowed_entry_in_room = !fields[4].empty() ? stoi(fields[4]) : 0; 
    message->plays_first = !fields[5].empty() ? stoi(fields[5]) : 0; 
    if (!fields[6].empty()) {
        message->cross_or_circle = (CorssOrCircle)stoi(fields[6]); 
    }
    if (!fields[7].empty()) {
        message->selected_coordinate = std::pair<int, int>();
        fields[7] = fields[7].substr(1, fields[7].size() - 2);
        std::vector<std::string> subfields = Serializable::split_fields(fields[7]);
        message->selected_coordinate.first = stoi(subfields[0]);
        message->selected_coordinate.first = stoi(subfields[1]);
    }
    if(!fields[8].empty()) {
        message->result = (GameResult)stoi(fields[8]);
    }
    return message;
}
