#ifndef DATABASE_H
#define DATABASE_H

#include "core/connection_pool.h" 
#include <string>             
#include <vector>             
#include <unordered_map>      
#include <functional>         

class Database {
public:
    explicit Database(ConnectionPool& pool);

    bool execute_query(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);
    std::vector<std::vector<std::string>> fetch_results(const std::string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams);

private:
    ConnectionPool& connectionPool;
    std::unordered_map<std::string, std::string> preparedStatements;

    bool check_query_error(PGresult* res);
    void prepare_statement(const std::string& stmtName, const std::string& query, int nParams);
    void prepare_queries();
    std::unique_ptr<PGconn, ConnectionPool::Deleter> get_connection_from_pool();

    struct QueryInfo {
        std::string stmtName;
        std::string query;
        int nParams;
    };

    static const std::function<void(PGresult*)> resultDeleter;
};

#endif // DATABASE_H