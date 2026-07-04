#include "multipart_parser.h"
#include <iostream>

using namespace std;

void MultipartParser::parse(const string& content_type, const string& body, HttpRequest& request) {
    // 1. Extract the boundary string from the Content-Type header
    string boundary_prefix = "boundary=";
    size_t pos = content_type.find(boundary_prefix);
    if (pos == string::npos) return;
    
    string boundary = "--" + content_type.substr(pos + boundary_prefix.length());
    
    // 2. Locate the file payload between the boundaries
    size_t start = body.find(boundary);
    if (start == string::npos) return;
    
    start += boundary.length() + 2; // Move past boundary and \r\n
    
    size_t end_headers = body.find("\r\n\r\n", start);
    if (end_headers == string::npos) return;
    
    string part_headers = body.substr(start, end_headers - start);
    
    // 3. Extract filename
    string filename_prefix = "filename=\"";
    size_t fn_pos = part_headers.find(filename_prefix);
    string filename = "uploaded_file.dat"; // Fallback name
    
    if (fn_pos != string::npos) {
        size_t fn_end = part_headers.find("\"", fn_pos + filename_prefix.length());
        filename = part_headers.substr(fn_pos + filename_prefix.length(), fn_end - (fn_pos + filename_prefix.length()));
    }
    
    // 4. Extract raw file data
    size_t file_start = end_headers + 4;
    size_t file_end = body.find(boundary, file_start);
    if (file_end != string::npos) {
        file_end -= 2; // Remove the trailing \r\n before the final boundary
    }
    
    string file_data = body.substr(file_start, file_end - file_start);
    
    // 5. Store in HttpRequest
    UploadedFile file;
    file.filename = filename;
    file.data = file_data;
    request.set_file(file);
}