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

#endif
