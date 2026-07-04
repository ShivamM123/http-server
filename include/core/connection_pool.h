#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <libpq-fe.h> // PostgreSQL C API

class ConnectionPool {
private:
    std::queue<PGconn*> pool;
    std::mutex mtx;
    std::condition_variable cv;
    std::string conninfo;
    size_t pool_size;
    bool shutdown_flag;

    void initialize_pool();

public:
    ConnectionPool(size_t size, const std::string& connection_string);
    ~ConnectionPool();

    // Grab a connection from the pool (blocks if empty)
    PGconn* acquire();
    
    // Return a connection back to the pool
    void release(PGconn* conn);
};

#endif // CONNECTION_POOL_H