#include "server.h"

using namespace std;

#define BACKLOG 10 // How many pending incoming connections the OS should queue up

Server::Server(char* port) {
    // 1. Create and bind the socket
    server_sockfd = create_bind_socket(port);
    
    // 2. Start listening on that socket
    start_listening(server_sockfd, port);
    
    // 3. Temporarily, we will just create an infinite loop so the program doesn't exit.
    // In the next step, we will replace this loop with code that actually accepts clients!
    cout << "Server is running. Press Ctrl+C to stop.\n";
    while(true) {
        sleep(1); 
    }
}

Server::~Server() {
    // Always clean up the socket when the server shuts down
    close(server_sockfd);
}

int Server::create_bind_socket(char* port) {
    int sockfd{}; 
    struct addrinfo hints{};
    struct addrinfo* listOfAddr{};
    struct addrinfo* p{};

    // "hints" tells the OS what kind of socket we are looking for
    hints.ai_family = AF_UNSPEC;            // Don't care if it's IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;        // We want a TCP Stream Socket (reliable, two-way connection)
    hints.ai_flags = AI_PASSIVE;            // Use my local machine's IP address automatically

    // getaddrinfo() asks the OS for network interfaces that match our hints and port
    if ((getaddrinfo(NULL, port, &hints, &listOfAddr)) != 0) {
        throw runtime_error("Error getting address info\n");
    }

    // The OS returns a linked list of possible addresses. We loop through them and try to connect.
    for (p = listOfAddr; p != nullptr; p = p->ai_next) {
        
        // Step A: Ask OS for a socket endpoint
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Error creating a socket\n");
            continue; // If it fails, try the next address in the list
        }

        // Step B: Bind that socket to the specific port (e.g., 8080)
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("Error binding socket\n");
            close(sockfd); // Close the failed socket
            continue;      // Try the next address
        }
        
        // If both socket() and bind() succeeded, break out of the loop!
        break; 
    }

    // If 'p' is null, it means we looped through everything and failed to bind
    if (!p) {
        freeaddrinfo(listOfAddr);
        throw runtime_error("Error in binding: Could not bind to any address\n");
    }
    
    // Free the memory used by the linked list
    freeaddrinfo(listOfAddr); 
    return sockfd;
}

void Server::start_listening(int sockfd, char* port) {
    // listen() tells the OS to start accepting incoming TCP connections and put them in a queue
    if (listen(sockfd, BACKLOG) < 0) {
        throw runtime_error("Error in listening\n");
    }
    cout << "The server has started listening on port: " << port << '\n';
}