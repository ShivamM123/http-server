#include "connection_pool.h"
#include <iostream>
#include <stdexcept>

using namespace std;

ConnectionPool::ConnectionPool(size_t size, const string& connection_string) 
    : pool_size(size), conninfo(connection_string), shutdown_flag(false) {
    initialize_pool();
}

ConnectionPool::~ConnectionPool() {
    unique_lock<mutex> lock(mtx);
    shutdown_flag = true;
    
    while (!pool.empty()) {
        PGconn* conn = pool.front();
        pool.pop();
        PQfinish(conn); // Safely close the Postgres connection
    }
}

void ConnectionPool::initialize_pool() {
    for (size_t i = 0; i < pool_size; ++i) {
        PGconn* conn = PQconnectdb(conninfo.c_str());
        
        if (PQstatus(conn) != CONNECTION_OK) {
            string err = PQerrorMessage(conn);
            PQfinish(conn);
            throw runtime_error("Database connection failed: " + err);
        }
        pool.push(conn);
    }
    cout << "Database connection pool initialized with " << pool_size << " connections.\n";
}

PGconn* ConnectionPool::acquire() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this] { return !pool.empty() || shutdown_flag; });

    if (shutdown_flag) {
        throw runtime_error("Connection pool is shutting down");
    }

    PGconn* conn = pool.front();
    pool.pop();
    return conn;
}

void ConnectionPool::release(PGconn* conn) {
    if (!conn) return;
    
    {
        unique_lock<mutex> lock(mtx);
        pool.push(conn);
    }
    cv.notify_one(); // Wake up any thread waiting for a database connection
}