#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include "http_request.h"

class HttpParser {
public:
    // Takes raw socket bytes and decodes them into our HttpRequest object
    static HttpRequest parse(const std::string& raw_request);

private:
    // Helper to extract ?key=value parameters from the URL
    static void parse_query_params(const std::string& query_string, HttpRequest& request);
};

#endif // HTTP_PARSER_H