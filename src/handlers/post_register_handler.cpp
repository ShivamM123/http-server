#include "post_register_handler.h"
#include <vector>

using namespace std;

HttpResponse PostRegisterHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    string username = req.get_query_param("username");
    string password = req.get_query_param("password"); // In a production app, this must be hashed via bcrypt before insertion

    if (username.empty() || password.empty()) {
        res.set_status(400, "Bad Request");
        res.add_header("Content-Type", "text/plain");
        res.set_body("Error: Username and password are required.");
        return res;
    }

    try {
        vector<string> params = {username, password};
        // Secure execution using parameterized query to prevent SQL Injection
        auto db_res = db.execute_query("INSERT INTO users (username, password_hash) VALUES ($1, $2)", params);
        
        res.set_status(302, "Found");
        res.add_header("Location", "/login"); // Redirect to login page upon success
        res.set_body("Registration successful. Redirecting...");
    } 
    catch (const exception& e) {
        res.set_status(500, "Internal Server Error");
        res.add_header("Content-Type", "text/plain");
        res.set_body("Database Error: User might already exist.");
    }

    return res;
}