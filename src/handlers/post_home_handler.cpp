#include "post_home_handler.h"
#include "../http/multipart_parser.h"
#include "../middleware/auth.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h> // For mkdir

using namespace std;

HttpResponse PostHomeHandler::handle(const HttpRequest& req, Database& db) {
    HttpResponse res;
    
    if (!Auth::is_authenticated(req)) {
        res.set_status(401, "Unauthorized");
        res.set_body("Unauthorized");
        return res;
    }

    // Since req is const, we make a copy to run the parser on it
    HttpRequest request_copy = req;
    string content_type = req.get_header("Content-Type");
    
    if (content_type.find("multipart/form-data") != string::npos) {
        MultipartParser::parse(content_type, req.get_body(), request_copy);
    }
    
    UploadedFile file = request_copy.get_file();
    
    if (!file.filename.empty() && !file.data.empty()) {
        // Ensure uploads directory exists
        mkdir("uploads", 0777); 
        
        string filepath = "uploads/" + file.filename;
        ofstream out(filepath, ios::binary);
        if (out.is_open()) {
            out.write(file.data.c_str(), file.data.size());
            out.close();
            
            // Log to database (Assuming a 'history' table exists)
            string user_id = "1"; // In a real app, extract this from the session token
            db.execute_query("INSERT INTO history (user_id, action) VALUES ($1, $2)", {user_id, "Uploaded file: " + file.filename});
            
            res.set_status(200, "OK");
            res.set_body("File '" + file.filename + "' uploaded successfully!");
            return res;
        }
    }
    
    res.set_status(400, "Bad Request");
    res.set_body("File upload failed.");
    return res;
}