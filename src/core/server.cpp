#include "server.h"
#include "../http/http_parser.h"
#include "../handlers/get_register_handler.h"
#include "../handlers/post_register_handler.h"
#include "../handlers/get_login_handler.h"
#include "../handlers/post_login_handler.h"
#include "../handlers/get_home_handler.h"
#include "../handlers/post_logout_handler.h"
#include "../handlers/post_home_handler.h"
#include "../handlers/get_download_handler.h"
#include "../handlers/get_history_handler.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <csignal>

using namespace std;

#define MAX_BUFFER 65536
#define BACKLOG 10

static Server* serverInstance = nullptr;

void signalHandler(int signum) {
    cout << "\nInterrupt signal (" << signum << ") received. Shutting down gracefully...\n";
    if (serverInstance) {
        serverInstance->stop();
    }
}

Server::Server(const char* port) 
    : shutServer(false), 
      pool(4), 
      connPool(5, "dbname=postgres user=postgres password=postgres host=localhost port=5432"), 
      db(connPool) 
{
    serverInstance = this;
    setup_signal_handler();
    register_routes();

    server_sockfd = create_bind_socket(port);
    start_listening(server_sockfd, port);

    while (!shutServer.load()) {
        int client_socket = accept_connection(server_sockfd);
        if (client_socket != -1) {
            pool.submit([this, client_socket]() {
                this->handle_client(client_socket);
            });
        }
    }

    close(server_sockfd);
    cout << "Server shutdown complete.\n";
}

Server::~Server() {
    if (!shutServer.load()) stop();
    close(pipefd[0]);
    close(pipefd[1]);
    serverInstance = nullptr;
}

void Server::stop() {
    shutServer = true;
    char msg[] = "Q";
    if (write(pipefd[1], msg, strlen(msg) + 1) == -1) {
        perror("Error writing to pipe");
    }
}

void Server::setup_signal_handler() {
    if (pipe(pipefd) == -1) {
        throw runtime_error("Failed to create signal pipe");
    }
    signal(SIGINT, signalHandler);
}

int Server::create_bind_socket(const char* port) {
    int sockfd = -1;
    struct addrinfo hints{}, *listOfAddr{}, *p{};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &listOfAddr) != 0) {
        throw runtime_error("Error getting address info");
    }

    for (p = listOfAddr; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;
        
        int yes = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }

    freeaddrinfo(listOfAddr);
    if (!p) throw runtime_error("Failed to bind socket");

    return sockfd;
}

void Server::start_listening(int sockfd, const char* port) {
    if (listen(sockfd, BACKLOG) == -1) {
        throw runtime_error("Listen failed");
    }
    cout << "Server listening on port " << port << "...\n";
}

int Server::accept_connection(int sockfd) {
    fd_set ready_sockets;
    FD_ZERO(&ready_sockets);
    FD_SET(sockfd, &ready_sockets);
    FD_SET(pipefd[0], &ready_sockets);

    int maxFd = max(sockfd, pipefd[0]);

    if (select(maxFd + 1, &ready_sockets, NULL, NULL, NULL) < 0) {
        if (errno == EINTR) return -1;
        throw runtime_error("Select error");
    }

    if (FD_ISSET(sockfd, &ready_sockets)) {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        return client_sockfd;
    }

    return -1; 
}

void Server::handle_client(int client_socket) {
    char buffer[MAX_BUFFER];
    memset(buffer, 0, MAX_BUFFER);

    int bytes_received = recv(client_socket, buffer, MAX_BUFFER - 1, 0);
    
    if (bytes_received > 0) {
        string raw_request(buffer, bytes_received);
        HttpRequest req = HttpParser::parse(raw_request);
        HttpResponse res = router.handle_request(req, db);

        string raw_response = res.to_string();
        send(client_socket, raw_response.c_str(), raw_response.length(), 0);
    }

    close(client_socket);
}

void Server::register_routes() {
    // Registering all endpoints to their specific handlers
    router.add_route("GET", "/", [](const HttpRequest& req, Database& db) {
        HttpResponse res;
        res.set_status(301, "Moved Permanently");
        res.add_header("Location", "/login");
        res.set_body("");
        return res;
    });

    router.add_route("GET", "/register", GetRegisterHandler::handle);
    router.add_route("POST", "/register", PostRegisterHandler::handle);
    
    router.add_route("GET", "/login", GetLoginHandler::handle);
    router.add_route("POST", "/login", PostLoginHandler::handle);
    
    router.add_route("GET", "/home", GetHomeHandler::handle);
    router.add_route("POST", "/home", PostHomeHandler::handle);
    router.add_route("POST", "/logout", PostLogoutHandler::handle);
    
    router.add_route("GET", "/download", GetDownloadHandler::handle);
    router.add_route("GET", "/history", GetHistoryHandler::handle);
}