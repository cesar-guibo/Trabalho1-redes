#include "tictactoe/argparse.hpp"
#include "tictactoe/client.hpp"
#include "tictactoe/game_messages.hpp"
#include "tictactoe/game.hpp"
#include "tictactoe/socket_error.hpp"
#include "tictactoe/socket_address.hpp"
#include <string>
#include <iostream>
#include <chrono>
#include <vector>

int main(int argc, char *argv[]) 
{
    using namespace std::chrono_literals;
    
   	ArgParser parser;
	parser.add_positional("a");
	parser.add_positional("p");
	parser.add_default("p", "12345");
	parser.add_default("a", "127.0.0.1");
	parser.parse(argc, argv);

	std::string addr = parser.get_arg("a");
	unsigned short port = stoi(parser.get_arg("p"));

    SocketAddress server_address(addr, port);
    ClientConnector *client;
    GameMessage *message;
    try {
        client = new ClientConnector(server_address);
    } catch (std::exception const& e) {
        std::cout << 1 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    try{
        message = new GameMessage(); 
        message->type = MessageType::PLAYER_NAME;
        printf("Insert your name:\n");
        std::string name_temp;
        std::cin >> name_temp;
        message->player_name = name_temp;//std::string("someone");
        client->send(message);
        message->clear();
        delete message;
    } catch (std::exception const& e) {
        std::cout << 2 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    try{
        message = client->receive();
    } catch (std::exception const& e) {
        std::cout << 3 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    if(message->rooms.empty()){
        std::cout << "No rooms available. Input the number of the room you want:" << std::endl;
    }else{
        for (auto& room : message->rooms) {
            std::pair<std::string, std::string> players_name = room->get_players_name();
            std::cout << room->get_id() << ": " 
            << (players_name.first == Room::EMPTY_STR_FLAG ? players_name.first : "")
            << ", "
            << (players_name.second == Room::EMPTY_STR_FLAG ? players_name.second : "")
            << std::endl;
        }
        std::cout << "What room do you want to enter?" << std::endl;
    }
    delete message;
    try { 
         message = new GameMessage();
         message->type = MessageType::SELECTED_ROOM;
         //message->selected_room_id =
         std::cin >> message->selected_room_id;
         client->send(message);
         delete message;
     } catch (std::exception const& e) {
         std::cout << 4 << std::endl;
         std::cout << e.what() << std::endl;
         std::cout << std::endl;
     }
    try {
        message = client->receive();
        std::cout << "message received" << (message->allowed_entry_in_room) << std::endl;
        std::cout << (message->allowed_entry_in_room ? "Allowed to enter the room" : "You were denied entry to the room") 
            << std::endl;
        delete message;
    } catch (std::exception const& e) {
        std::cout << 5 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    try {
        message = client->receive();
        std::cout << (message->plays_first ? "You play first" : "You play second")
            << std::endl 
            << "You will play with "
            << (message->cross_or_circle == CrossOrCircle::CROSS ? "X" : "O")
            << std::endl;
            bool plays_first = message->plays_first;
        CrossOrCircle cross_or_circle = message->cross_or_circle;
        delete message;
        try {
            if (plays_first) {
                message = new GameMessage();
                message->type = MessageType::EXECUTED_PLAY;
                message->selected_coordinate = std::pair<int, int>(1, 1);
                client->send(message);
                delete message;
            } else {
                message = client->receive();
                std::cout << "(" << message->selected_coordinate.first << ","
                    << message->selected_coordinate.second << ")" << std::endl;
                delete message;
            }
        } catch (std::exception const& e) {
            std::cout << 7 << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << std::endl;
        }
    } catch (std::exception const& e) {
        std::cout << 6 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    while(1){

    }
     
    // try {
    //     message = client->receive();
    //     std::cout << (message->result == GameResult::WON
    //             ? "You won!" : message->result == GameResult::LOST
    //             ? "You lost!" : "The match ended in a draw.");
    //     delete message;
    // } catch(std::exception const& e) {
    //     std::cout << 7 << std::endl;
    //     std::cout << e.what() << std::endl;
    //     std::cout << std::endl;
    // }
    
    /* O or X 
    std::vector<char> board;*/

	return 0;
}
