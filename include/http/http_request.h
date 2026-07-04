#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

// Structure to securely hold multipart/form-data uploads
struct UploadedFile {
    std::string filename;
    std::string content_type;
    std::string data;
};

class HttpRequest {
private:
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;
    std::string body;
    UploadedFile file;

public:
    HttpRequest() = default;
    ~HttpRequest() = default;

    // Setters
    void set_method(const std::string& m);
    void set_path(const std::string& p);
    void set_version(const std::string& v);
    void add_header(const std::string& key, const std::string& value);
    void add_query_param(const std::string& key, const std::string& value);
    void set_body(const std::string& b);
    void set_file(const UploadedFile& f);

    // Getters
    std::string get_method() const;
    std::string get_path() const;
    std::string get_version() const;
    std::string get_header(const std::string& key) const;
    std::string get_query_param(const std::string& key) const;
    std::string get_body() const;
    UploadedFile get_file() const;
};

#endif // HTTP_REQUEST_H