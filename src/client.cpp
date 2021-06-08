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

    while(true) {
        message = client->receive();
        if(message->rooms.empty()){
            std::cout << "No rooms available."
            << " Input the number of the room you want to create:" << std::endl;
        }else{
            for (auto& room : message->rooms) {
                std::pair<std::string, std::string> players_name = room->get_players_name();
                std::cout << room->get_id() << ": " 
                << (players_name.first == Room::EMPTY_STR_FLAG ? "Empty" : players_name.first)
                << ", "
                << (players_name.second == Room::EMPTY_STR_FLAG ? "Empty" : players_name.second)
                << std::endl;
            }
            std::cout << "What room do you want to enter?" << std::endl;
        }
        message->clear();
        message->type = MessageType::SELECTED_ROOM;
        std::cin >> message->selected_room_id;
        message->plays_first = true;
        client->send(message);
        delete message;
        message = client->receive();
        if (message->allowed_entry_in_room) {
            std::cout << "Allowed to enter the room" << std::endl;
            break;
        }
        std::cout << "You were denied entry to the room" << std::endl;
        delete message;
    }

    bool plays_first;
    CrossOrCircle cross_or_circle;
    try {
        message = client->receive();
        std::cout << (message->plays_first ? "You play first" : "You play second")
            << std::endl 
            << "You will play with "
            << (message->cross_or_circle == CrossOrCircle::CROSS ? "X" : "O")
            << std::endl;
        plays_first = message->plays_first;
        cross_or_circle = message->cross_or_circle;
        delete message;
    } catch (std::exception const& e) {
        std::cout << 6 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }

    Board board;
    std::cout << board.to_string();
    if (plays_first) {
        std::cout << "Make your move: " << std::endl;
        message = new GameMessage();
        message->type = MessageType::EXECUTED_PLAY;

        std::cin >> message->selected_coordinate.first;
        std::cin >> message->selected_coordinate.second;
        board.mark_chosen(cross_or_circle, message->selected_coordinate);
        std::cout << board.to_string();

        client->send(message);
        delete message;
    }
    while(board.get_state() == BoardState::NOT_FINISHED) {
        try {
            std::cout << "Waiting for your opponents move" << std::endl;
            message = client->receive();
            board.mark_chosen(cross_or_circle, message->selected_coordinate);
            std::cout << board.to_string();

            std::cout << "Make your move:" << std::endl;
            message->clear();
            message->type = MessageType::EXECUTED_PLAY;
            std::cin >> message->selected_coordinate.first;
            std::cin >> message->selected_coordinate.second;
            board.mark_chosen(cross_or_circle, message->selected_coordinate);
            std::cout << board.to_string();
            client->send(message);
            delete message;
        } catch (std::exception const& e) {
            std::cout << 7 << std::endl;
            std::cout << e.what() << std::endl;
            std::cout << std::endl;
        }
    }
    std::cout << "The game has ended" << std::endl;
     
    try {
        message = client->receive();
        std::cout << (message->result == GameResult::WON
                ? "You won!" : message->result == GameResult::LOST
                ? "You lost!" : "The match ended in a draw.");
        delete message;
    } catch(std::exception const& e) {
        std::cout << 7 << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << std::endl;
    }
    
    /* O or X 
    std::vector<char> board;*/

	return 0;
}
