#include "socket_address.hpp"
#include <arpa/inet.h>

SocketAddress::SocketAddress(const std::string &ipv4, unsigned short port)
{
    address.sin_family = AF_INET;
    inet_aton(ipv4.c_str(), (struct in_addr *)&address.sin_addr.s_addr);
    address.sin_port = htons(port);
}

const struct sockaddr *SocketAddress::c_sockaddr() {
    return (struct sockaddr *)&address;
}
