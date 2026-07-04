#include "database.h"
#include <iostream>
#include <stdexcept>

using namespace std;

Database::Database(ConnectionPool& connection_pool) : pool(connection_pool) {}

SafePGresult Database::execute_query(const string& query, const vector<string>& params) {
    PGconn* conn = pool.acquire();
    
    // Prepare parameter arrays for libpq C API
    int nParams = params.size();
    vector<const char*> paramValues;
    paramValues.reserve(nParams);
    
    for (const auto& param : params) {
        paramValues.push_back(param.c_str());
    }

    // Execute the query using parameterized inputs to prevent SQL injection
    PGresult* res = PQexecParams(
        conn,
        query.c_str(),
        nParams,
        nullptr, // Let the backend deduce param types
        paramValues.empty() ? nullptr : paramValues.data(),
        nullptr, // Parameter lengths (not needed for text)
        nullptr, // Parameter formats (0 = text)
        0        // Result format (0 = text)
    );

    // Immediately wrap the raw pointer in our smart pointer so it auto-cleans on return/throw
    SafePGresult safe_res(res);

    // Return the connection to the pool regardless of query success/failure
    pool.release(conn);

    ExecStatusType status = PQresultStatus(safe_res.get());
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        string err = PQerrorMessage(conn);
        cerr << "Query execution failed: " << err << "\n";
        throw runtime_error("Database query failed: " + err);
    }

    return safe_res;
}