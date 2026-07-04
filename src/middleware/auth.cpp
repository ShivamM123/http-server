#include "auth.h"

using namespace std;

bool Auth::is_authenticated(const HttpRequest& req) {
    string cookie_header = req.get_header("Cookie");
    
    // Simple verification: checks if a session_token exists in the cookies
    if (cookie_header.find("session_token=") != string::npos) {
        return true; 
    }
    return false;
}

string Auth::extract_session_token(const HttpRequest& req) {
    string cookie_header = req.get_header("Cookie");
    size_t pos = cookie_header.find("session_token=");
    
    if (pos != string::npos) {
        size_t end = cookie_header.find(";", pos);
        if (end == string::npos) {
            end = cookie_header.length();
        }
        // Extract the value immediately following "session_token="
        return cookie_header.substr(pos + 14, end - (pos + 14));
    }
    return "";
}