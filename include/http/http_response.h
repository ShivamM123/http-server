#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <map>

#include <string>
#include <map>

class HttpResponse {
private:
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    HttpResponse();
    ~HttpResponse() = default;

    void set_status(int code, const std::string& message);
    void add_header(const std::string& key, const std::string& value);
    void set_body(const std::string& b);

    // Serializes the object into a raw TCP string ready for socket transmission
    std::string to_string() const;
};

#endif // HTTP_RESPONSE_H