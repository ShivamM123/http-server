#include "http/http_request.h"

using namespace std;

string HttpRequest::get_body() const { return body; }
unordered_map<string, string> HttpRequest::get_headers() const { return header; }
string HttpRequest::get_specific_header(const string& key) const { return header.contains(key) ? header.at(key) : ""; }
string HttpRequest::get_http_version() const { return http_version; }
string HttpRequest::get_method() const { return method; }
string HttpRequest::get_path() const { return path; }
string HttpRequest::get_unparsed_query_params() const { return unparsedQuery; }
string HttpRequest::get_token_cookie() const { return token; }
string HttpRequest::get_query_param(const string& key) const { return queryParams.contains(key) ? queryParams.at(key) : ""; }
unordered_map<string, string> HttpRequest::get_query_map() const { return queryParams; }
string HttpRequest::get_boundary_string() const { return boundaryString; }
unordered_map<string, HttpRequest::UploadedFile> HttpRequest::get_all_files() const { return uploaded_files; }
unordered_map<string, HttpRequest::UploadedFile>& HttpRequest::get_all_files_ref() { return uploaded_files; }
string HttpRequest::get_specific_form_field(const string& key) const { return form_fields.contains(key) ? form_fields.at(key) : ""; }
string HttpRequest::get_file_name(const string& field_name) const { return has_file(field_name) ? uploaded_files.at(field_name).filename : ""; }
string HttpRequest::get_file_perms(const string& field_name) const { return has_file(field_name) ? uploaded_files.at(field_name).perms : "private"; }
vector<uint8_t> HttpRequest::get_file_content(const string& field_name) const { return has_file(field_name) ? uploaded_files.at(field_name).content : vector<uint8_t>{}; }
string HttpRequest::as_text(const vector<uint8_t>& content) const { return string(content.begin(), content.end()); }
HttpRequest::FileType HttpRequest::get_file_type(const string& field_name) const { return has_file(field_name) ? uploaded_files.at(field_name).type : HttpRequest::FileType::BINARY; }
size_t HttpRequest::get_file_size(const string& field_name) const { return has_file(field_name) ? uploaded_files.at(field_name).size : 0; }

void HttpRequest::set_method(const string& meth) { method = meth; }
void HttpRequest::set_http_version(const string& ver) { http_version = ver; }
void HttpRequest::set_path(const string& urlpath) { path = urlpath; }
void HttpRequest::set_body(const string& reqBody) { body = reqBody; }
void HttpRequest::set_header(const unordered_map<string, string>& head) { header = head; }
void HttpRequest::set_specific_header(const string& key, const string& value) { header[key] = value; }
void HttpRequest::set_unparsed_query_string(const string& qString) { unparsedQuery = qString; }
void HttpRequest::set_specific_query_param(const string& key, const string& value) { queryParams[key] = value; }
void HttpRequest::set_token_cookie(const string& tokenFromCookie) { token = tokenFromCookie; }
void HttpRequest::set_boundary_string(const string& boundary) { boundaryString = boundary; }
void HttpRequest::set_specific_form_field(const string& field_name, const string& value) { form_fields[field_name] = value; }
void HttpRequest::update_body(const string& newBody) { body = newBody; }

void HttpRequest::add_file(const string& fieldName, const string& filename, const vector<uint8_t>& content, const HttpRequest::FileType type, const string& perm) {
    UploadedFile newFile;
    newFile.filename = filename;
    newFile.content = content;
    newFile.type = type;
    newFile.perms = perm;
    newFile.size = content.size();
    uploaded_files[fieldName] = newFile;
}

void HttpRequest::clear() {
    method.clear();
    path.clear();
    http_version.clear();
    body.clear();
    token.clear();
    boundaryString.clear();
    header.clear();
    queryParams.clear();
    form_fields.clear();
    uploaded_files.clear();
}

bool HttpRequest::has_body() const { return !body.empty(); }
bool HttpRequest::has_header(const string& key) const { return header.contains(key); }
bool HttpRequest::has_query_params() const { return !queryParams.empty(); }
bool HttpRequest::has_specific_query_param(const string& key) const { return queryParams.contains(key); }
bool HttpRequest::has_boundary_string() const { return !boundaryString.empty(); }
bool HttpRequest::has_form_field(const string& field_name) const { return form_fields.contains(field_name); }
bool HttpRequest::has_file(const string& field_name) const { return uploaded_files.contains(field_name); }

bool HttpRequest::full_body_received() const {
    if (method == "GET") return true;
    if (!header.contains("content-length")) return false;

    try {
        size_t expectedLength = stoul(header.at("content-length"));
        return body.length() == expectedLength;
    } catch (const exception&) {
        return false; 
    }
}