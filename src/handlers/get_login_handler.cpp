#include "get_login_handler.h"

using namespace std;

HttpResponse GetLoginHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    res.set_status(200, "OK");
    res.add_header("Content-Type", "text/html");
    
    string html = 
        "<html><head><style>"
        "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap');"
        "body { font-family: 'Inter', sans-serif; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color: white; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; }"
        ".card { background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); padding: 40px; border-radius: 15px; box-shadow: 0 4px 15px rgba(0,0,0,0.2); width: 100%; max-width: 400px; text-align: center; border: 1px solid rgba(255,255,255,0.2); }"
        "input[type='text'], input[type='password'] { width: 90%; padding: 12px; margin: 10px 0; border-radius: 8px; border: none; outline: none; background: rgba(255,255,255,0.8); }"
        "input[type='submit'] { background: linear-gradient(to right, #00d2ff, #3a7bd5); color: white; padding: 12px 24px; border: none; border-radius: 8px; font-weight: 600; cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; text-decoration: none; display: inline-block; margin-top: 15px; width: 100%; }"
        "input[type='submit']:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0, 210, 255, 0.4); }"
        "a { color: #00d2ff; text-decoration: none; margin-top: 15px; display: inline-block; font-size: 0.9em; }"
        "a:hover { text-decoration: underline; }"
        "</style></head><body>"
        "<div class='card'>"
        "<h2>User Login</h2>"
        "<form method='POST' action='/login'>"
        "<input type='text' name='username' placeholder='Username' required><br>"
        "<input type='password' name='password' placeholder='Password' required><br>"
        "<input type='submit' value='Login'>"
        "</form>"
        "<a href='/register'>Don't have an account? Register</a>"
        "</div></body></html>";
        
    res.set_body(html);
    return res;
}