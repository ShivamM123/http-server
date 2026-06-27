#include "http/http_response.h"
#include <stdexcept>

using namespace std;

const unordered_map<int, string> HttpResponse::codeToMessage = {
    // 2xx Success
    {200, "OK"},
    {201, "Created"},
    {202, "Accepted"},
    {204, "No Content"},
    
    // 3xx Redirection
    {301, "Moved Permanently"},
    {302, "Found"},
    {303, "See Other"},
    {304, "Not Modified"},
    
    // 4xx Client Errors
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {405, "Method Not Allowed"},
    {408, "Request Timeout"},
    {409, "Conflict"},
    {410, "Gone"},

    // 5xx Server Errors
    {500, "Internal Server Error"},
    {501, "Not Implemented"},
    {502, "Bad Gateway"},
    {503, "Service Unavailable"},
    {504, "Gateway Timeout"}
};

void HttpResponse::set_status(int code, string message) {
    if(code < 100 || code >= 600){
        throw runtime_error("Invalid status code");
    }

    status_code = code;
    
    if(codeToMessage.count(code) == 0 && message.size() == 0){
        throw runtime_error("Unknown status code without message");
    } else {
        status_message = (!message.empty()) ? message : codeToMessage.at(code);
    }
}

void HttpResponse::set_header(const string& key, const string& value) {
    headers[key] = value;
}

void HttpResponse::set_body(const string& content) {
    body = content;
}

string HttpResponse::get_body() {
    return body;
}

string HttpResponse::get_string_response() const {
    string response{};
    string http_version = "HTTP/1.1 ";

    // 1. Status Line (e.g., HTTP/1.1 200 OK)
    response += http_version + to_string(status_code) + " " + status_message + "\r\n";
    
    // 2. Headers
    for(const auto& [key, value] : headers) {
        response += key + ": " + value + "\r\n"; 
    }
    
    // 3. Blank line indicating end of headers
    response += "\r\n";
    
    // 4. Body payload
    response += body;

    return response;
}