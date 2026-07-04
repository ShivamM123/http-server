#include "get_home_handler.h"
#include "../middleware/auth.h"

using namespace std;

HttpResponse GetHomeHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    // Security Middleware Check
    if (!Auth::is_authenticated(req)) {
        res.set_status(302, "Found");
        res.add_header("Location", "/login");
        res.set_body("Unauthorized access. Redirecting to login...");
        return res;
    }

    res.set_status(200, "OK");
    res.add_header("Content-Type", "text/html");
    
    string html = 
        "<html><body>"
        "<h2>Welcome to your Dashboard!</h2>"
        "<p>You are securely logged in.</p>"
        "<form method='POST' action='/logout'>"
        "<input type='submit' value='Logout'>"
        "</form></body></html>";
        
    res.set_body(html);
    return res;
}