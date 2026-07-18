#include "post_login_handler.h"
#include <vector>
#include <libpq-fe.h> // For PQntuples and PQgetvalue
#include "crypto_util.h"

using namespace std;

HttpResponse PostLoginHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    string username = req.get_query_param("username");
    string password = req.get_query_param("password");

    if (username.empty() || password.empty()) {
        res.set_status(400, "Bad Request");
        res.add_header("Content-Type", "text/plain");
        res.set_body("Error: Credentials required.");
        return res;
    }

    try {
        string hashed_password = CryptoUtil::hash_password(password);
        vector<string> params = {username, hashed_password};
        // Queries the database to find a matching user
        auto db_res = db.execute_query("SELECT id FROM users WHERE username = $1 AND password_hash = $2", params);
        
        if (PQntuples(db_res.get()) > 0) {
            // Success: User found. Create a cryptographically signed session token.
            string user_id = PQgetvalue(db_res.get(), 0, 0);
            string session_token = CryptoUtil::generate_session_token(user_id);
            
            res.set_status(302, "Found");
            res.add_header("Location", "/home");
            // Set the HTTP-only cookie
            res.add_header("Set-Cookie", "session_token=" + session_token + "; HttpOnly; Path=/");
            res.set_body("Login successful. Redirecting to home...");
        } else {
            // Failure: No match found
            res.set_status(401, "Unauthorized");
            res.add_header("Content-Type", "text/plain");
            res.set_body("Error: Invalid username or password.");
        }
    } 
    catch (const exception& e) {
        res.set_status(500, "Internal Server Error");
        res.add_header("Content-Type", "text/plain");
        res.set_body("Database Error during login.");
    }

    return res;
}