#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <netinet/in.h>

class SocketAddress {
    public:
        SocketAddress(const std::string &ipv4, unsigned short server_port);
        const struct sockaddr *c_sockaddr();
    private:
        struct sockaddr_in address;
};

#endif
