#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        perror("Failed to create socket\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(argv[1], (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = htons((unsigned short) atoi(argv[2]));
    if (connect(client_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }
    printf("Connection was established successfully\n.");
    return 0;
}
