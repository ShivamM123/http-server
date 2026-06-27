#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <queue>               
#include <mutex>               
#include <condition_variable>  
#include <memory>              
#include <libpq-fe.h>          
#include <chrono>              
#include <iostream>            
#include <string>

class ConnectionPool {
public:
    // Custom deleter for PGconn objects to ensure memory safety
    struct Deleter {           
        void operator()(PGconn* conn) {
            if (conn) {
                PQfinish(conn);
            }
        }
    };

    ConnectionPool(size_t poolSize, const std::string& connInfo, int timeout);
    ~ConnectionPool();

    std::unique_ptr<PGconn, Deleter> get_connection();
    void return_connection(std::unique_ptr<PGconn, Deleter> conn);
    static bool check_pq_status(const std::unique_ptr<PGconn, Deleter>& conn);
    size_t get_pool_size();

private:
    std::queue<std::unique_ptr<PGconn, Deleter>> availableConnections;
    std::mutex poolMutex;
    std::condition_variable poolCV;
    size_t poolSize;
    std::string connInfo;
    std::chrono::milliseconds timeoutDuration;

    std::unique_ptr<PGconn, Deleter> make_new_connection();
    void initializePool();
};

#endif // CONNECTION_POOL_H