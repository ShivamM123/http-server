#ifndef AUTH_H
#define AUTH_H

#include "../http/http_request.h"
#include <string>

class Auth {
public:
    // Checks if the incoming request has a valid session token
    static bool is_authenticated(const HttpRequest& req);
    
    // Extracts the session token from the Cookie header
    static std::string extract_session_token(const HttpRequest& req);
};

#endif // AUTH_H