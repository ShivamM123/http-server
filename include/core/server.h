#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <atomic>
#include "thread_pool.h"
#include "connection_pool.h"
#include "../database/database.h"
#include "../http/router.h"

class Server {
private:
    int server_sockfd;
    int pipefd[2]; // Self-pipe for graceful shutdown via select()
    std::atomic<bool> shutServer;
    
    ThreadPool pool;
    ConnectionPool connPool;
    Database db;
    Router router;

    int create_bind_socket(const char* port);
    void start_listening(int sockfd, const char* port);
    int accept_connection(int sockfd);
    void handle_client(int client_socket);
    void setup_signal_handler();
    void register_routes(); // Where you will define your API endpoints

public:
    Server(const char* port);
    ~Server();
    void stop();
};

#endif // SERVER_H