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
    try {
        client = new ClientConnector(server_address);
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    std::cout << "Starting the game" << std::endl;
    ClientGameState *state = new WaitForNameInput(client);
    while(!state->is_last_state()) {
        state->start();
        ClientGameState *aux = state;
        state = state->next();
        delete aux;
    }
    std::cout << "Exiting the game" << std::endl;

    delete client;

	return 0;
}
