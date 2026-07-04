#include "get_history_handler.h"
#include "../middleware/auth.h"
#include <libpq-fe.h>

using namespace std;

HttpResponse GetHistoryHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    if (!Auth::is_authenticated(req)) {
        res.set_status(401, "Unauthorized");
        res.set_body("Unauthorized");
        return res;
    }

    res.set_status(200, "OK");
    res.add_header("Content-Type", "text/html");
    
    string html = 
        "<html><head><style>"
        "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;600&display=swap');"
        "body { font-family: 'Inter', sans-serif; background: linear-gradient(135deg, #0f2027, #203a43, #2c5364); color: white; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 100vh; margin: 0; }"
        ".card { background: rgba(255, 255, 255, 0.1); backdrop-filter: blur(10px); padding: 40px; border-radius: 15px; box-shadow: 0 4px 15px rgba(0,0,0,0.2); width: 100%; max-width: 600px; text-align: center; border: 1px solid rgba(255,255,255,0.2); }"
        "ul { list-style: none; padding: 0; text-align: left; }"
        "li { background: rgba(0,0,0,0.2); padding: 15px; margin-bottom: 10px; border-radius: 8px; border-left: 4px solid #00d2ff; }"
        "a { color: white; text-decoration: none; display: inline-block; background: linear-gradient(to right, #3a7bd5, #3a6073); padding: 12px 24px; border-radius: 8px; font-weight: 600; margin-top: 20px; transition: transform 0.2s; }"
        "a:hover { transform: translateY(-2px); }"
        "</style></head><body>"
        "<div class='card'>"
        "<h2>Your Activity History 📋</h2><ul>";
    
    try {
        // Fetch history from DB
        auto db_res = db.execute_query("SELECT action, created_at FROM history WHERE user_id = $1 ORDER BY created_at DESC LIMIT 50", {"1"});
        int rows = PQntuples(db_res.get());
        
        if (rows == 0) {
            html += "<li>No activity found.</li>";
        } else {
            for (int i = 0; i < rows; ++i) {
                string action = PQgetvalue(db_res.get(), i, 0);
                string date = PQgetvalue(db_res.get(), i, 1);
                html += "<li><strong>" + date + "</strong><br>" + action + "</li>";
            }
        }
    } catch (...) {
        html += "<li>Error loading history.</li>";
    }
    
    html += "</ul><a href='/home'>Back to Home</a></div></body></html>";
    res.set_body(html);
    return res;
}