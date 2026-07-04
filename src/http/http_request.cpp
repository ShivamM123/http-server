#include "http_request.h"

using namespace std;

void HttpRequest::set_method(const string& m) { method = m; }
void HttpRequest::set_path(const string& p) { path = p; }
void HttpRequest::set_version(const string& v) { version = v; }
void HttpRequest::set_body(const string& b) { body = b; }
void HttpRequest::set_file(const UploadedFile& f) { file = f; }

void HttpRequest::add_header(const string& key, const string& value) {
    string lower_key = key;
    for (char& c : lower_key) c = tolower(c);
    headers[lower_key] = value;
}

void HttpRequest::add_query_param(const string& key, const string& value) {
    query_params[key] = value;
}

string HttpRequest::get_method() const { return method; }
string HttpRequest::get_path() const { return path; }
string HttpRequest::get_version() const { return version; }
string HttpRequest::get_body() const { return body; }
UploadedFile HttpRequest::get_file() const { return file; }

string HttpRequest::get_header(const string& key) const {
    string lower_key = key;
    for (char& c : lower_key) c = tolower(c);
    auto it = headers.find(lower_key);
    if (it != headers.end()) {
        return it->second;
    }
    return "";
}

string HttpRequest::get_query_param(const string& key) const {
    auto it = query_params.find(key);
    if (it != query_params.end()) {
        return it->second;
    }
    return "";
}