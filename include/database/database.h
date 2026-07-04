#ifndef DATABASE_H
#define DATABASE_H

#include "../core/connection_pool.h"
#include <string>
#include <vector>
#include <memory>

// Custom deleter for PGresult to be used with std::unique_ptr
struct PGresultDeleter {
    void operator()(PGresult* res) const {
        if (res) {
            PQclear(res);
        }
    }
};

// Type alias for safe, auto-cleaning PostgreSQL results
using SafePGresult = std::unique_ptr<PGresult, PGresultDeleter>;

class Database {
private:
    ConnectionPool& pool;

public:
    Database(ConnectionPool& connection_pool);
    ~Database() = default;

    // Executes a prepared statement safely
    SafePGresult execute_query(const std::string& query, const std::vector<std::string>& params = {});
};

#endif // DATABASE_H