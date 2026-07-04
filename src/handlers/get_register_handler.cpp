#include "get_register_handler.h"

using namespace std;

HttpResponse GetRegisterHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    res.set_status(200, "OK");
    res.add_header("Content-Type", "text/html");
    
    string html = 
        "<html><body>"
        "<h2>Register a New Account</h2>"
        "<form method='POST' action='/register'>"
        "Username: <input type='text' name='username' required><br>"
        "Password: <input type='password' name='password' required><br>"
        "<input type='submit' value='Register'>"
        "</form></body></html>";
        
    res.set_body(html);
    return res;
}