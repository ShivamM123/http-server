#ifndef MULTIPART_PARSER_H
#define MULTIPART_PARSER_H

#include <string>
#include "http_request.h"

class MultipartParser {
public:
    // Parses a multipart form body and populates the UploadedFile struct inside HttpRequest
    static void parse(const std::string& content_type, const std::string& body, HttpRequest& request);
};

#endif // MULTIPART_PARSER_H