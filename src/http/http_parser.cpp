#include "http/http_parser.h"
#include <iostream>

using namespace std;

HttpRequest HttpParser::parse(const string& raw_request){
    
    string headerPart{};
    string bodyPart{};

    // Split at the first occurrence of \r\n\r\n
    size_t pos = raw_request.find("\r\n\r\n");
    
    if(pos != string::npos){
        headerPart = raw_request.substr(0, pos);
        bodyPart = raw_request.substr(pos+4); // +4 moves past \r\n\r\n
    }
    else{
        headerPart = raw_request; // No body present
    }

    vector<string> headerLines = split_lines(headerPart);
    if(headerLines.empty()) throw runtime_error("Invalid request: no headers");

    HttpRequest req;

    // Line 0 contains method, path, http version
    parse_request_line(headerLines[0], req);                                                         
    
    // Store the rest of the headers as key-value pairs
    parse_headers(headerLines, req);                                                                 
    
    if(req.has_header("cookie")){
        parse_token_from_header(req.get_specific_header("cookie"), req);    
    } else {
        req.set_token_cookie("");
    }

    // Check if http request contains multipart file data
    if(req.has_header("content-type") && req.get_specific_header("content-type").starts_with("multipart/form-data")){
        parse_boundary_string(req.get_specific_header("content-type"), req);
        req.set_body(bodyPart);
    } else {
        req.set_body(bodyPart);                                                                         
    }
    
    return req;
}

void HttpParser::parse_boundary_string(const string& header, HttpRequest& req) {
    size_t startPos = header.find("boundary=");
    if(startPos == string::npos) {
        req.set_boundary_string("");
    } else {
        startPos += string("boundary=").length();
        string boundary = header.substr(startPos);
        req.set_boundary_string(boundary);
    }
}

vector<string> HttpParser::split_lines(const string& raw) {
    vector<string> headerLines;
    size_t startPos = 0;
    string line{};

    while(true) {   
        size_t endPos = raw.find("\r\n", startPos);
        if(endPos == string::npos){
            line = raw.substr(startPos);
            line = Util::trim(line);

            if(!line.empty()) headerLines.push_back(line);
            break;
        }

        line = raw.substr(startPos, endPos - startPos);
        line = Util::trim(line);

        if(!line.empty()) headerLines.push_back(line);

        startPos = endPos + 2;
    }

    return headerLines;
}

void HttpParser::parse_token_from_header(const string& cookie_header, HttpRequest& req) {
    string token_prefix = "token=";
    size_t start_pos = cookie_header.find(token_prefix);
    
    if (start_pos != string::npos) {
        start_pos += token_prefix.length();
        size_t end_pos = cookie_header.find(";", start_pos);
        if (end_pos == string::npos) {
            end_pos = cookie_header.length();
        }
        req.set_token_cookie(cookie_header.substr(start_pos, end_pos - start_pos));
    } else {
        req.set_token_cookie("");
    }
}


void HttpParser::parse_query_params(const string& path, HttpRequest& req){
    size_t queryPos = path.find("?");
    if(queryPos == string::npos) return;

    string unparsedQueryString = path.substr(queryPos+1);
    req.set_unparsed_query_string(unparsedQueryString);
    
    stringstream stream(unparsedQueryString);
    string keyValueString{};
    
    while(getline(stream, keyValueString, '&')) {
        size_t equalToPos = keyValueString.find("=");
        string key{};
        string value{};
        
        if(equalToPos == string::npos){
            key = keyValueString;
            value = "";    
        } else {
            key = keyValueString.substr(0, equalToPos);
            value = keyValueString.substr(equalToPos + 1);
        }
        
        key = Util::trim(key);
        value = Util::trim(value);
        key = Util::url_decode(key);
        value = Util::url_decode(value);
        
        req.set_specific_query_param(key, value);
    }
}

void HttpParser::parse_request_line(const string& line, HttpRequest& req) {   
    string method{}, path{}, version{};
    istringstream stream(line);
    
    stream >> method >> path >> version;
    
    if (method.empty() || path.empty() || version.empty()) {
        throw runtime_error("Invalid request line");
    }

    req.set_method(method);
    req.set_path(path);
    req.set_http_version(version);

    parse_query_params(path, req);
}

void HttpParser::parse_headers(const vector<string>& headerPart, HttpRequest& req) {
    unordered_map<string, string> header;

    for(size_t i = 1; i < headerPart.size(); ++i) {   
        size_t pos = headerPart[i].find(":");
        if(pos == string::npos) continue; // Skip malformed headers without colons
        
        string key = headerPart[i].substr(0, pos);          
        string value = headerPart[i].substr(pos + 1);        

        key = Util::trim(key);
        transform(key.begin(), key.end(), key.begin(), ::tolower); 
        value = Util::trim(value);

        header[key] = value;
    }
    req.set_header(header);
}