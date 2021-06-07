#ifndef SOCKET_ERROR_H
#define SOCKET_ERROR_H

#include <stdexcept>
#include <cerrno>
#include <cstring>

class SocketError : std::runtime_error
{
    public:
        SocketError() : std::runtime_error("") {}
        const char *what() {
            return strerror(errno);
        }
};

#endif
