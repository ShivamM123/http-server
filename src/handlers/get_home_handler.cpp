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
        "<html><head><style>"
        "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap');"
        "body { font-family: 'Inter', sans-serif; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color: white; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; }"
        ".card { background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); padding: 40px; border-radius: 15px; box-shadow: 0 4px 15px rgba(0,0,0,0.2); width: 100%; max-width: 500px; text-align: center; border: 1px solid rgba(255,255,255,0.2); }"
        "input[type='text'], input[type='file'] { width: 90%; padding: 12px; margin: 10px 0; border-radius: 8px; border: none; outline: none; background: rgba(255,255,255,0.8); }"
        "input[type='submit'], .btn { background: linear-gradient(to right, #00d2ff, #3a7bd5); color: white; padding: 12px 24px; border: none; border-radius: 8px; font-weight: 600; cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; text-decoration: none; display: inline-block; margin-top: 15px; width: 100%; }"
        "input[type='submit']:hover, .btn:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0, 210, 255, 0.4); }"
        "a { color: #00d2ff; text-decoration: none; margin-top: 15px; display: inline-block; font-size: 0.9em; }"
        "a:hover { text-decoration: underline; }"
        "hr { border: 0; height: 1px; background: rgba(255,255,255,0.2); margin: 25px 0; }"
        "</style></head><body>"
        "<div class='card'>"
        "<h2>Welcome to your Dashboard 🚀</h2>"
        "<p style='color: #b0c4de; font-size: 0.9em;'>You are securely logged in.</p>"
        "<hr>"
        "<h3>Upload a File</h3>"
        "<form method='POST' action='/home' enctype='multipart/form-data'>"
        "<input type='file' name='file' required><br>"
        "<input type='submit' value='Upload File'>"
        "</form>"
        "<hr>"
        "<h3>Download a File</h3>"
        "<form method='GET' action='/download'>"
        "<input type='text' name='file' placeholder='Filename (e.g. image.png, data.txt)' required><br>"
        "<input type='submit' value='Download File'>"
        "</form>"
        "<hr>"
        "<a href='/history' class='btn' style='width: auto;'>View Action History</a>"
        "<form method='POST' action='/logout' style='margin-top: 10px;'>"
        "<input type='submit' value='Logout' style='background: linear-gradient(to right, #ff416c, #ff4b2b);'>"
        "</form>"
        "</div></body></html>";
        
    res.set_body(html);
    return res;
}