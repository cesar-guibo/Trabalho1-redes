#include "client.hpp"
#include "socket_error.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream>

#define MESSAGE_MAX_SIZE 500

ClientConnector::ClientConnector(SocketAddress &server_address) noexcept(false)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0)
        throw SocketError();
    const struct sockaddr *c_address = server_address.c_sockaddr();
    if(connect(socket_fd, c_address, sizeof(*c_address)) < 0)
        throw SocketError();
}

ClientConnector::~ClientConnector() noexcept(false)
{
    if (shutdown(socket_fd, SHUT_RDWR) < 0)
        throw SocketError();
}

void ClientConnector::send(GameMessage *message) noexcept(false)
{
    std::string message_str = message->serialize();
    int bytes_sent = ::send(socket_fd, message_str.c_str(), message_str.size(), 0);
    if (bytes_sent < (int)message_str.size())
        throw SocketError();
}

GameMessage *ClientConnector::receive() noexcept(false)
{
    char buffer[MESSAGE_MAX_SIZE];
    int bytes_received = ::recv(socket_fd, buffer, MESSAGE_MAX_SIZE, 0);
    if (bytes_received < 0)
        throw SocketError();
    buffer[bytes_received] = '\0';
    std::cout << buffer << std::endl;
    return GameMessage::parse(std::string(buffer));
}

ClientGameState::ClientGameState() {}
ClientGameState::~ClientGameState() {}
ClientGameState *ClientGameState::next() { return new ClientGameState(); }
void ClientGameState::start() {}
bool ClientGameState::is_last_state() { return false; }

WaitForNameInput::WaitForNameInput(ClientConnector *connector)
{
    this->connector = connector;
    connection_interrupted = false;
}
WaitForNameInput::~WaitForNameInput() {}

ClientGameState *WaitForNameInput::next() 
{
    if (connection_interrupted)
        return new Quit();
    return new DisplayRooms(connector);
}

void WaitForNameInput::start()
{
    std::string name;
    std::cout << "Type the name you wish to use: ";
    std::cin >> name;
    GameMessage *message = new GameMessage();
    message->type = MessageType::PLAYER_NAME;
    message->player_name = name;
    try {
        connector->send(message);
    } catch(const std::exception &e) {
        connection_interrupted = true;
        std::cout << e.what();
    }
    delete message;
}

bool WaitForNameInput::is_last_state()
{
    return false;
}

DisplayRooms::DisplayRooms(ClientConnector *connector) 
{
    this->connector = connector;
    connection_interrupted = false;
}
DisplayRooms::~DisplayRooms() {}

ClientGameState *DisplayRooms::next() 
{
    if (connection_interrupted)
        return new Quit();
    return new WaitForRoomSelection(connector, rooms.empty());
}

std::string DisplayRooms::rooms_to_string()
{
    std::stringstream ss;
    for (std::shared_ptr<Room> room : rooms) {
        ss << room->get_id() << " | ";
        std::pair<std::string, std::string> names = room->get_players_name();
        ss << "Player 1: ";
        if (names.first != Room::EMPTY_STR_FLAG)
             ss << names.first;
        else 
            ss << "EMPTY";
        ss << ", Player 2: ";
        if (names.second != Room::EMPTY_STR_FLAG)
            ss << names.second;
        else 
            ss << "EMPTY";
    }
    return ss.str();
}

void DisplayRooms::start()
{
    GameMessage *message;
    try {
        message = connector->receive();
        int max_tries = 10;
        while(message->type != MessageType::AVAILABLE_ROOMS and max_tries > 0) {
            delete message;
            message = connector->receive();
            if (message->type == MessageType::CONNECTION_CLOSED) {
                std::cout << "There was a problem with the connection." << std::endl;
                connection_interrupted = true;
                break;
            }
        }
    } catch (const std::exception &e){
        connection_interrupted = true;
        std::cout << e.what();
    }
    rooms = message->rooms;
    if (message->rooms.empty()) {
        std::cout << "There are currently no rooms." << std::endl;
    } else {
        std::cout << rooms_to_string() << std::endl; 
    }
}

bool DisplayRooms::is_last_state()
{
    return false;
}


WaitForRoomSelection::WaitForRoomSelection
(ClientConnector *connector, bool no_rooms) 
{
    this->connector = connector;
    connection_interrupted = false;
    this->no_rooms = no_rooms;
}

WaitForRoomSelection::~WaitForRoomSelection() {}

ClientGameState *WaitForRoomSelection::next() 
{
    if (connection_interrupted)
        return new Quit();
    return new WaitForTheStartOfTheGame(connector);
}

void WaitForRoomSelection::start()
{
    GameMessage *message;
    if (no_rooms) {
        std::cout << "Input the number of the room you want to create: ";
    } else {
        std::cout << "Select a room to play in: ";
    }
    while (true) {
        int selected;
        std::cin >> selected;
        message = new GameMessage();
        message->type = MessageType::SELECTED_ROOM;
        message->selected_room_id = selected;
        try {
            connector->send(message);
        } catch (const std::exception &e) {
            connection_interrupted = true;
            std::cout << e.what();
        }
        delete message;

        try {
            message = connector->receive();
        } catch (const std::exception &e) {
            connection_interrupted = true;
            std::cout << e.what();
        }
        if (message->type == MessageType::CONNECTION_CLOSED) {
            std::cout << "There was a problem with the connection." << std::endl;
            connection_interrupted = true;
            break;
        }
        if (message->type == MessageType::ENTERED_ROOM
                and message->allowed_entry_in_room) {
            break;
        } else {
            std::cout << "This room is not available, select another one: ";
        }
    }
}

bool WaitForRoomSelection::is_last_state()
{
    return false;
}

WaitForTheStartOfTheGame::WaitForTheStartOfTheGame(ClientConnector *connector)
{
    this->connector = connector;
    connection_interrupted = false;
}
WaitForTheStartOfTheGame::~WaitForTheStartOfTheGame() {}

ClientGameState *WaitForTheStartOfTheGame::next()
{
    if (connection_interrupted)
        return new Quit();
    return new PlayGame(connector, cross_or_circle, plays_first);
}

void WaitForTheStartOfTheGame::start()
{
    int max_tries = 10;
    GameMessage *message;
    try {
        message = connector->receive();
        while(message->type != MessageType::GAME_STARTED and max_tries > 0) {
            delete message;
            message = connector->receive();
            if (message->type == MessageType::CONNECTION_CLOSED) {
                std::cout << "There was a problem with the connection." << std::endl;
                connection_interrupted = true;
                break;
            }
        }
    } catch (const std::exception &e) {
        connection_interrupted = true;
        std::cout << e.what();
    }
    plays_first = message->plays_first;
    cross_or_circle = message->cross_or_circle;
    std::cout << "The game has started" << std::endl;
}

bool WaitForTheStartOfTheGame::is_last_state()
{
    return false;
}

PlayGame::PlayGame
(ClientConnector *connector, CrossOrCircle cross_or_circle, bool plays_first)
{
    this->connector = connector;
    this->cross_or_circle = cross_or_circle;
    this->plays_first = plays_first;
    game_ended = false;
    connection_interrupted = false;
    board = new Board();
}

PlayGame::~PlayGame() {
    delete board;
}

ClientGameState *PlayGame::next()
{
    if (connection_interrupted)
        return new Quit();
    return new GameEnded(connector, result);
}

bool PlayGame::client_won()
{
    BoardState state = board->get_state();
    bool circle = cross_or_circle == CrossOrCircle::CIRCLE
        and state == BoardState::CIRCLE_WON; 
    bool cross = cross_or_circle == CrossOrCircle::CROSS
        and state == BoardState::CROSS_WON;
    return circle or cross;
}

void PlayGame::execute_play()
{
    std::cout << board->to_string() << std::endl;
    std::cout << "Select an available position: ";
    std::pair<int, int> coordinate;
    std::cin >> coordinate.first >> coordinate.second;
    board->mark_chosen(cross_or_circle, coordinate);
    std::cout << board->to_string() << std::endl;
    GameMessage *message = new GameMessage();
    if (client_won()) {
        result = GameResult::WON;
        game_ended = true;
        message->type = MessageType::GAME_ENDED;
        message->result = result;
    } else if (board->get_state() == BoardState::TIED) {
        result = GameResult::TIED;
        game_ended = true;
        message->type = MessageType::GAME_ENDED;
        message->result = result;
    } else {
        message->type = MessageType::EXECUTED_PLAY;
    }
    message->selected_coordinate = coordinate;

    try {
        connector->send(message);
    } catch(const std::exception &e) {
        connection_interrupted = true;
        std::cout << e.what();
    }
    delete message;
}

void PlayGame::wait_for_opponents_play()
{
    GameMessage *message = nullptr;
    try {
        message = connector->receive();
        int max_tries = 10;
        while(message->type != MessageType::GAME_STARTED and max_tries > 0) {
            delete message;
            message = connector->receive();
            if (message->type == MessageType::CONNECTION_CLOSED) {
                connection_interrupted = true;
                break;
            }
            if (message->type == MessageType::GAME_ENDED) {
                game_ended = true;
                result = message->result;
                break;
            }
        }
    } catch (const std::exception &e) {
        connection_interrupted = true;
        if (message == nullptr)
            delete message;
        std::cout << e.what();
        return;
    }
    CrossOrCircle opponent = cross_or_circle == CrossOrCircle::CROSS ?
        CrossOrCircle::CIRCLE : CrossOrCircle::CROSS;
    board->mark_chosen(opponent, message->selected_coordinate);
    std::cout << "Received te oppnents play" << std::endl;
    std::cout << board->to_string() << std::endl;
}

void PlayGame::start()
{
    int max_tries = 10;
    if (plays_first)
        execute_play();
    while(!game_ended and !connection_interrupted) {
        wait_for_opponents_play();
        if (game_ended or connection_interrupted)
            break;
        execute_play();
    }
}

bool PlayGame::is_last_state()
{
    return false;
}

GameEnded::GameEnded
(ClientConnector *connector, GameResult result)
{
    this->connector = connector;
    this->result = result;
}

GameEnded::~GameEnded() {}

ClientGameState *GameEnded::next()
{
    if (quit)
        return new Quit();
    return new DisplayRooms(connector);
}

void GameEnded::start()
{
    std::cout << "The game has ended, ";
    switch (result) {
        case GameResult::WON: 
            std::cout << "YOU WON!";
        case GameResult::LOST:
            std::cout << "you lost.";
        case GameResult::TIED:
            std::cout << "It came to a draw.";
    }
    std::cout << std::endl << std::endl;
    std::cout << "Would you like to play another match? (Y/n)" << std::endl;
    char in;
    std::cin >> in;
    quit = in == 'Y' or in == 'y';
}

bool GameEnded::is_last_state()
{
    return false;
}

Quit::Quit() {}
Quit::~Quit() {}
ClientGameState *Quit::next() { return new Quit(); }
void Quit::start() {}
bool Quit::is_last_state()
{
    return true;
}
