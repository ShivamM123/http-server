#include "auth.h"
#include "crypto_util.h"

using namespace std;

bool Auth::is_authenticated(const HttpRequest& req) {
    string token = extract_session_token(req);
    if (token.empty()) {
        return false;
    }
    
    // Verify the signature of the session token
    return CryptoUtil::verify_token(token);
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