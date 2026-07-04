#include "http_parser.h"
#include "../utility/util.h"
#include <iostream>

using namespace std;

HttpRequest HttpParser::parse(const string& raw_request) {
    HttpRequest request;
    
    if (raw_request.empty()) {
        return request;
    }

    // Split the raw request into lines based on carriage return + newline
    vector<string> lines = Util::split(raw_request, "\r\n");
    if (lines.empty()) return request;

    // 1. Parse the Request Line (e.g., "GET /api/users?id=5 HTTP/1.1")
    vector<string> request_line = Util::split(lines[0], " ");
    if (request_line.size() >= 3) {
        request.set_method(Util::trim(request_line[0]));
        
        string full_path = Util::trim(request_line[1]);
        request.set_version(Util::trim(request_line[2]));

        // Check if the path contains query parameters
        size_t query_pos = full_path.find('?');
        if (query_pos != string::npos) {
            request.set_path(full_path.substr(0, query_pos));
            string query_string = full_path.substr(query_pos + 1);
            parse_query_params(query_string, request);
        } else {
            request.set_path(full_path);
        }
    }

    // 2. Parse Headers
    size_t i = 1;
    for (; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            break; // A blank line indicates the end of headers
        }
        
        size_t colon_pos = lines[i].find(':');
        if (colon_pos != string::npos) {
            string key = Util::trim(lines[i].substr(0, colon_pos));
            string value = Util::trim(lines[i].substr(colon_pos + 1));
            request.add_header(key, value);
        }
    }

    // 3. Parse Body (Everything after the blank line)
    string body = "";
    i++; // Skip the blank line
    for (; i < lines.size(); ++i) {
        body += lines[i];
        if (i < lines.size() - 1) {
            body += "\r\n"; // Reconstruct internal newlines if any
        }
    }
    request.set_body(body);

    return request;
}

void HttpParser::parse_query_params(const string& query_string, HttpRequest& request) {
    vector<string> pairs = Util::split(query_string, "&");
    for (const auto& pair : pairs) {
        size_t equals_pos = pair.find('=');
        if (equals_pos != string::npos) {
            string key = pair.substr(0, equals_pos);
            string value = pair.substr(equals_pos + 1);
            request.add_query_param(key, value);
        }
    }
}