#include "get_download_handler.h"
#include "../middleware/auth.h"
#include <fstream>
#include <sstream>

using namespace std;

HttpResponse GetDownloadHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    if (!Auth::is_authenticated(req)) {
        res.set_status(401, "Unauthorized");
        res.set_body("Unauthorized");
        return res;
    }

    string filename = req.get_query_param("file");
    if (filename.empty()) {
        res.set_status(400, "Bad Request");
        res.set_body("Missing file parameter.");
        return res;
    }

    // Security measure: Prevent directory traversal attacks (e.g. file=../../../etc/passwd)
    if (filename.find("..") != string::npos || filename.find("/") != string::npos) {
        res.set_status(403, "Forbidden");
        res.set_body("Invalid filename.");
        return res;
    }

    string filepath = "uploads/" + filename;
    ifstream file(filepath, ios::binary);
    
    if (file.is_open()) {
        stringstream buffer;
        buffer << file.rdbuf();
        
        res.set_status(200, "OK");
        res.add_header("Content-Type", "application/octet-stream");
        res.add_header("Content-Disposition", "attachment; filename=\"" + filename + "\"");
        res.set_body(buffer.str());
        
        // Log to database
        db.execute_query("INSERT INTO history (user_id, action) VALUES ($1, $2)", {"1", "Downloaded file: " + filename});
    } else {
        res.set_status(404, "Not Found");
        res.set_body("File not found on server.");
    }

    return res;
}