#include "connection_pool.h"

using namespace std;

// Constructor
ConnectionPool::ConnectionPool(size_t poolSize, const string& connInfo, int timeout) 
    : poolSize(poolSize), connInfo(connInfo), timeoutDuration(chrono::milliseconds(timeout)) {
    cout << "Initializing connection pool with " << poolSize << " connections...\n";
    initializePool();
    cout << "Connection pool initialized successfully with " << poolSize << " connections.\n";
}

// Destructor
ConnectionPool::~ConnectionPool() {
    cout << "Closing the connection pool with " << availableConnections.size() << " connections.\n";
    // Because we are using smart pointers, the unique_ptrs remaining in the queue
    // will automatically invoke their custom Deleter (PQfinish) when they go out of scope.
}

size_t ConnectionPool::get_pool_size() {
    return poolSize;
}

// Creates the initial batch of connections
void ConnectionPool::initializePool() {
    // No need to lock poolMutex here since this runs sequentially in the constructor 
    // before any threads are spawned or given access to the pool.
    for (size_t i = 0; i < poolSize; ++i) {
        unique_ptr<PGconn, ConnectionPool::Deleter> conn(PQconnectdb(connInfo.c_str()), ConnectionPool::Deleter());
        if (check_pq_status(conn)) {
            availableConnections.push(move(conn));
        } else {
            cerr << i << "/" << poolSize << " connections made before failure.\n";
            throw runtime_error("Error in constructing the pool: " + string(PQerrorMessage(conn.get())));
        }
    }
}

// Returns true on successful connection, and false otherwise
bool ConnectionPool::check_pq_status(const unique_ptr<PGconn, ConnectionPool::Deleter>& conn) {
    if (PQstatus(conn.get()) != CONNECTION_OK) {
        return false;
    }
    return true;
}

// Replaces a dead socket with a fresh connection
unique_ptr<PGconn, ConnectionPool::Deleter> ConnectionPool::make_new_connection() {
    unique_ptr<PGconn, ConnectionPool::Deleter> conn(PQconnectdb(connInfo.c_str()), ConnectionPool::Deleter());
    if (check_pq_status(conn)) {
        cout << "A replacement connection was created successfully.\n";
        return conn;
    } else {
        cerr << "Error in finding a replacement connection." << endl;
        return nullptr;
    }
}

// Called by worker threads to check out a connection
unique_ptr<PGconn, ConnectionPool::Deleter> ConnectionPool::get_connection() {
    // Using unique_lock so it unlocks safely if an exception is thrown
    unique_lock<mutex> lock(poolMutex);

    cout << "Waiting for a database connection..." << endl;
    try {
        // Blocks thread, releasing the mutex until notified OR timeout expires
        bool status = poolCV.wait_for(lock, timeoutDuration, [this] { return !availableConnections.empty(); });

        if (!status) {
            cout << "Connection acquisition timed out.\n";
            throw runtime_error("Connection timed out\n");
        }

        // Transfer ownership from the pool to the requesting thread
        unique_ptr<PGconn, ConnectionPool::Deleter> conn(move(availableConnections.front()));
        availableConnections.pop();

        // The socket might have dropped while sitting idle in the queue. Validate it.
        if (!check_pq_status(conn)) {
            cout << "Invalid connection found, replacing it...\n";
            conn = make_new_connection();
            
            if (!conn) {
                throw runtime_error("Error in finding a replacement connection");
            }
        }

        cout << "Connection allocated!" << endl;
        return conn;
    } catch (const exception& e) {
        cout << "Error in getting a connection: " << e.what() << endl;
        throw;
    }
}

// Called by worker threads to return ownership back to the pool
void ConnectionPool::return_connection(unique_ptr<PGconn, ConnectionPool::Deleter> conn) {
    cout << "Received the connection back.\n";

    unique_lock<mutex> lock(poolMutex);

    // Ensure the thread didn't somehow break the connection during execution
    if (!check_pq_status(conn)) {
        cout << "The returned connection is now invalid, creating a replacement...\n";
        conn = make_new_connection();
        if (!conn) {
            cerr << "Replacement failed, working with 1 less connection in the pool now.\n";
            return; // Exit early, dropping the pointer (invokes deleter)
        }
    }

    // Move ownership back into the queue
    availableConnections.push(move(conn));
    cout << "Connection returned to the pool successfully.\n";
    
    // Notify exactly one waiting thread that a connection is now available
    poolCV.notify_one();
}