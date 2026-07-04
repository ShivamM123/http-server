#include "http_response.h"

using namespace std;

HttpResponse::HttpResponse() : status_code(200), status_message("OK") {
    // Default headers
    add_header("Content-Type", "text/plain");
    add_header("Connection", "close");
}

void HttpResponse::set_status(int code, const string& message) {
    status_code = code;
    status_message = message;
}

void HttpResponse::add_header(const string& key, const string& value) {
    headers[key] = value;
}

void HttpResponse::set_body(const string& b) {
    body = b;
    // Automatically calculate and inject the Content-Length header
    add_header("Content-Length", to_string(body.length()));
}

string HttpResponse::to_string() const {
    string response = "HTTP/1.1 " + std::to_string(status_code) + " " + status_message + "\r\n";
    
    for (const auto& header : headers) {
        response += header.first + ": " + header.second + "\r\n";
    }
    
    response += "\r\n"; // Blank line separating headers from body
    response += body;
    
    return response;
}