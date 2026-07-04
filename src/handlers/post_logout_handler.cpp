#include "post_logout_handler.h"

using namespace std;

HttpResponse PostLogoutHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    res.set_status(302, "Found");
    res.add_header("Location", "/login");
    
    // Clear the session cookie by setting its Max-Age to 0
    res.add_header("Set-Cookie", "session_token=; HttpOnly; Path=/; Max-Age=0");
    res.set_body("Logging out...");
    
    return res;
}