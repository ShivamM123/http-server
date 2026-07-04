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
    
    string html = "<html><body><h2>Your Activity History</h2><ul>";
    
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
                html += "<li>" + date + " - " + action + "</li>";
            }
        }
    } catch (...) {
        html += "<li>Error loading history.</li>";
    }
    
    html += "</ul><a href='/home'>Back to Home</a></body></html>";
    res.set_body(html);
    return res;
}