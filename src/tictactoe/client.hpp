#ifndef CLIENT_H
#define CLIENT_H

#include "game_messages.hpp"
#include "socket_address.hpp"
#include <string>

class ClientConnector
{
    public:
        ClientConnector(SocketAddress &server_address) noexcept(false);
        ~ClientConnector() noexcept(false);
        void send(GameMessage *message) noexcept(false);
        GameMessage *receive() noexcept(false);

    private:
        int socket_fd;
};

class ClientGameState
{
    public:
        ClientGameState();
        virtual ~ClientGameState();
        virtual ClientGameState *next();
        virtual void start() noexcept(false);
        virtual bool is_last_state();
};

class WaitForNameInput : public ClientGameState
{
    public:
        WaitForNameInput(ClientConnector *connector);
        ~WaitForNameInput();
        ClientGameState *next();
        void start() noexcept(false);
        bool is_last_state();

    private:
        std::string name;
        ClientConnector *connector;
        bool connection_interrupted;
};

class DisplayRooms : public ClientGameState
{
    public:
        DisplayRooms(ClientConnector *connector);
        ~DisplayRooms();
        ClientGameState *next();
        void start();
        bool is_last_state();

    private:
        ClientConnector *connector;
        bool connection_interrupted;
        std::vector<std::shared_ptr<Room>> rooms;
        std::string rooms_to_string();
};

class WaitForRoomSelection : public ClientGameState
{
    public:
        WaitForRoomSelection(ClientConnector *connector, bool no_rooms);
        ~WaitForRoomSelection();
        ClientGameState *next();
        void start();
        bool is_last_state();

    private:
        ClientConnector *connector;
        bool connection_interrupted;
        bool no_rooms;
};

class WaitForTheStartOfTheGame : public ClientGameState
{
    public:
        WaitForTheStartOfTheGame(ClientConnector *connector);
        ~WaitForTheStartOfTheGame();
        ClientGameState *next();
        void start();
        bool is_last_state();

    private:
        ClientConnector *connector;
        bool plays_first;
        CrossOrCircle cross_or_circle;
        bool connection_interrupted;
};

class PlayGame : public ClientGameState
{
    public:
        PlayGame(ClientConnector *connector,
                 CrossOrCircle cross_or_circle,
                 bool plays_first);
        ~PlayGame();
        ClientGameState *next();
        void start() noexcept(false);
        bool is_last_state();

    private:
        ClientConnector *connector;
        CrossOrCircle cross_or_circle;
        bool plays_first;
        bool game_ended;
        bool connection_interrupted;
        GameResult result;
        Board *board;
        void execute_play();
        void wait_for_opponents_play();
        bool client_won();
};

class GameEnded : public ClientGameState
{
    public:
        GameEnded(ClientConnector *connector, GameResult result);
        ~GameEnded();
        ClientGameState *next();
        void start() noexcept(false);
        bool is_last_state();

    private:
        ClientConnector *connector;
        GameResult result;
        bool quit;
};

class Quit : public ClientGameState
{
    public:
        Quit();
        ~Quit();
        ClientGameState *next();
        void start() noexcept(false);
        bool is_last_state();

    private:
        ClientConnector *connector;
};

#endif
