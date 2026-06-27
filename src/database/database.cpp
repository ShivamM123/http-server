#include "database/database.h" // Adjust include path based on your folder structure
#include <iostream>

using namespace std;

// Static member definition for automatic PGresult memory cleanup
const function<void(PGresult*)> Database::resultDeleter = [](PGresult* res) { PQclear(res); };

// Constructor
Database::Database(ConnectionPool& pool) : connectionPool(pool) {
    prepare_queries();
}

unique_ptr<PGconn, ConnectionPool::Deleter> Database::get_connection_from_pool() {
    unique_ptr<PGconn, ConnectionPool::Deleter> conn(connectionPool.get_connection());

    // Ensure connection is valid before handing it to the caller
    if (!connectionPool.check_pq_status(conn)) {
        connectionPool.return_connection(move(conn));
        throw runtime_error("Error getting a connection from the pool: " + string(PQerrorMessage(conn.get())));
    }
    return conn;
}

void Database::prepare_queries() {
    // Hold all the queries to prepare ahead of time
    static const vector<Database::QueryInfo> queryList = {
        // User-related queries
        {"user_reg", "INSERT INTO users (username, password_hash) VALUES ($1, $2);", 2},
        {"user_login", "SELECT id, password_hash FROM users WHERE username = $1;", 1},
        {"user_name_available", "SELECT COUNT(*) FROM users WHERE username = $1;", 1},

        // Refresh token queries
        {"insert_refresh_token", "INSERT INTO user_sessions (user_id, refresh_token_hash, expires_at) VALUES ($1, $2, $3);", 3},
        {"get_refresh_token", "SELECT user_id, expires_at FROM user_sessions WHERE refresh_token_hash = $1;", 1},
        {"delete_refresh_token", "DELETE FROM user_sessions WHERE refresh_token_hash = $1;", 1},
        {"delete_all_tokens_for_user", "DELETE FROM user_sessions WHERE user_id = $1;", 1},

        // File-related queries
        {"file_insertion", "INSERT INTO files (user_id, filename, file_size, download_link, is_compressed, permission, filetype) VALUES ($1, $2, $3, $4, $5, $6, $7);", 7},
        {"file_retrieval", "SELECT user_id, filename, file_size, upload_timestamp, download_link, is_compressed, permission, filetype FROM files WHERE download_link = $1;", 1},
        {"file_retrieval_by_id", "SELECT id, filename, file_size, upload_timestamp, download_link, is_compressed, permission, filetype FROM files WHERE user_id = $1;", 1},
        {"file_compression", "UPDATE files SET is_compressed = $1 WHERE id = $2;", 2},
        {"file_deletion", "DELETE FROM files WHERE id = $1;", 1},

        // User info and file count
        {"user_info", "SELECT id, username, password_hash FROM users WHERE username = $1;", 1},
        {"count_user_file", "SELECT COUNT(*) FROM files WHERE user_id = $1;", 1},
        {"all_files", "SELECT id, filename, file_size, upload_timestamp, download_link, is_compressed, permission, filetype FROM files WHERE user_id = $1 ORDER BY upload_timestamp DESC;", 1},

        {"update_file_permission", "UPDATE files SET permission = $1 WHERE id = $2 AND user_id = $3;", 3},
        {"update_filetype", "UPDATE files SET filetype = $1 WHERE id = $2 AND user_id = $3;", 3}
    };

    // Preparing statements are connection specific, so we must prepare for EACH connection in the pool
    for (size_t i = 0; i < connectionPool.get_pool_size(); ++i) {
        unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());
        
        for (const auto& v : queryList) {
            unique_ptr<PGresult, decltype(resultDeleter)> res(
                PQprepare(conn.get(), v.stmtName.c_str(), v.query.c_str(), v.nParams, nullptr),
                resultDeleter
            );

            if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
                connectionPool.return_connection(move(conn));
                throw runtime_error("Error in preparing the query: " + string(PQresultErrorMessage(res.get())));
            }
        }
        // Safely return the connection to the queue
        connectionPool.return_connection(move(conn));
    }
}

void Database::prepare_statement(const string& stmtName, const string& query, int nParams) {
    unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    // decltype deduces the correct type of our custom lambda resultDeleter
    unique_ptr<PGresult, decltype(resultDeleter)> res(PQprepare(conn.get(), stmtName.c_str(), query.c_str(), nParams, nullptr), resultDeleter);

    if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
        connectionPool.return_connection(move(conn));
        throw runtime_error("Error in preparing the query: " + string(PQresultErrorMessage(res.get())));
    }

    preparedStatements[stmtName] = query;
    connectionPool.return_connection(move(conn));
}

// Execute queries without expecting a table payload (e.g., INSERT, UPDATE, DELETE)
bool Database::execute_query(const string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams) {
    unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    unique_ptr<PGresult, decltype(resultDeleter)> res(PQexecPrepared(conn.get(), stmtName.c_str(), nParams, params, paramLen, paramFormat, 0), resultDeleter);
    cout << "Executing query: " << stmtName << "\nParameters: " << params << endl;

    if (PQresultStatus(res.get()) != PGRES_COMMAND_OK) {
        connectionPool.return_connection(move(conn));
        throw runtime_error("Error in executing the db command: " + string(PQresultErrorMessage(res.get())));
    }

    cout << "Query executed successfully\n";
    connectionPool.return_connection(move(conn));
    return true;
}

// Execute SELECT queries and serialize into a 2D vector matrix
vector<vector<string>> Database::fetch_results(const string& stmtName, const char* params[], int paramLen[], int paramFormat[], int nParams) {
    unique_ptr<PGconn, ConnectionPool::Deleter> conn(get_connection_from_pool());

    unique_ptr<PGresult, decltype(resultDeleter)> res(PQexecPrepared(conn.get(), stmtName.c_str(), nParams, params, paramLen, paramFormat, 0), resultDeleter);
    cout << "Executing query: " << stmtName << "\nParameters: " << params << endl;

    if (PQresultStatus(res.get()) != PGRES_TUPLES_OK) {
        connectionPool.return_connection(move(conn));
        throw runtime_error("Error in executing the db command: " + string(PQresultErrorMessage(res.get())));
    }

    int numCol = PQnfields(res.get());
    int numRow = PQntuples(res.get());
    vector<vector<string>> resultVector;

    for (int i = 0; i < numRow; ++i) {
        vector<string> rowVector;
        for (int j = 0; j < numCol; ++j) {
            rowVector.push_back(PQgetisnull(res.get(), i, j) ? "NULL" : PQgetvalue(res.get(), i, j));
        }
        resultVector.push_back(rowVector);
    }

    cout << "Query executed successfully\n";
    connectionPool.return_connection(move(conn));
    return resultVector;
}