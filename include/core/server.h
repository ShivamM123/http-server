#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h> // Core socket functions
#include <netdb.h>      // For getaddrinfo (translating ports/IPs)
#include <unistd.h>     // For close() and sleep()
#include <stdexcept>    // For throwing errors

class Server {
public:
    Server(char* port);
    ~Server();

private:
    int server_sockfd; // This integer is a "File Descriptor" that represents our network connection

    int create_bind_socket(char* port);
    void start_listening(int sockfd, char* port);
};

#endif // SERVER_H