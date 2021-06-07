#include "client.hpp"
#include "socket_error.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

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
    if (bytes_sent < message_str.size())
        throw SocketError();
}

GameMessage *ClientConnector::receive() noexcept(false)
{
    char buffer[MESSAGE_MAX_SIZE];
    int bytes_received = ::recv(socket_fd, buffer, MESSAGE_MAX_SIZE, 0);
    if (bytes_received < 0)
        throw SocketError();
    buffer[bytes_received] = '\0';
    return GameMessage::parse(std::string(buffer));
}
