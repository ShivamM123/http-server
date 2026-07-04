#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include "http/http_request.h" 
#include "utility/util.h"         
#include <string>         
#include <vector>         
#include <sstream>        
#include <unordered_map>  
#include <algorithm>      

class HttpParser
{
public:
    HttpParser() = delete;                                    
    static HttpRequest parse(const std::string &raw_request); 

private:
    static std::vector<std::string> split_lines(const std::string &raw); 
    static void parse_request_line(const std::string &line, HttpRequest &req);               
    static void parse_headers(const std::vector<std::string> &headerPart, HttpRequest &req); 
    static void parse_query_params(const std::string &path, HttpRequest &req);               
    static void parse_token_from_header(const std::string &cookie_header, HttpRequest &req); 
    static void parse_boundary_string(const std::string &header, HttpRequest &req);          
    // static void parse_multipart_body(const std::string &bodyPart, HttpRequest &req); 
};

#endif // HTTP_PARSER_H