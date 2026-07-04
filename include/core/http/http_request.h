#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <iostream>      
#include <string>        
#include <cstdint>       
#include <unordered_map> 
#include <vector>        

class HttpRequest
{
public:
    enum class FileType { 
        TEXT,
        BINARY
    };

    struct UploadedFile {                                        
        std::string filename{};              
        std::vector<std::uint8_t> content{}; 
        std::string perms{"private"};        
        std::string downloadLink{};          
        double size{};                       
        FileType type{FileType::BINARY};     
    };
    
    UploadedFile file; 

    HttpRequest() = default; 

    // Getters
    std::string get_body() const;                                                    
    std::unordered_map<std::string, std::string> get_headers() const;                
    std::string get_specific_header(const std::string &key) const;                   
    std::string get_http_version() const;                                            
    std::string get_method() const;                                                  
    std::string get_path() const;                                                    
    std::string get_unparsed_query_params() const;                                   
    std::string get_query_param(const std::string &key) const;                       
    std::unordered_map<std::string, std::string> get_query_map() const;              
    std::string get_token_cookie() const;                                            
    std::string get_boundary_string() const;                                         
    std::string get_specific_form_field(const std::string &key) const;               
    std::string get_file_name(const std::string &field_name) const;                  
    std::string get_file_perms(const std::string &field_name) const;                 
    std::vector<std::uint8_t> get_file_content(const std::string &field_name) const; 
    std::string as_text(const std::vector<std::uint8_t> &content) const;             
    FileType get_file_type(const std::string &field_name) const;                     
    size_t get_file_size(const std::string &field_name) const;                       
    std::unordered_map<std::string, UploadedFile> get_all_files() const;             
    std::unordered_map<std::string, UploadedFile> &get_all_files_ref();              

    // Setters
    void set_method(const std::string &meth);                                        
    void set_http_version(const std::string &ver);                                   
    void set_path(const std::string &urlpath);                                       
    void set_body(const std::string &reqBody);                                       
    void set_header(const std::unordered_map<std::string, std::string> &head);       
    void set_specific_header(const std::string &key, const std::string &value);      
    void set_unparsed_query_string(const std::string &qString);                      
    void set_specific_query_param(const std::string &key, const std::string &value); 
    void set_token_cookie(const std::string &token);                                 
    void set_boundary_string(const std::string &boundary);                           
    void set_specific_form_field(const std::string &key, const std::string &value);  
    void add_file(const std::string &fieldName, const std::string &filename, const std::vector<std::uint8_t> &content, const HttpRequest::FileType type, const std::string &perm = "private"); 
    void update_body(const std::string &body);                                                                                      

    // Utilities
    void clear();                                                
    bool has_body() const;                                       
    bool has_header(const std::string &key) const;               
    bool has_query_params() const;                               
    bool has_specific_query_param(const std::string &key) const; 
    bool has_boundary_string() const;                            
    bool has_form_field(const std::string &key) const;           
    bool has_file(const std::string &field_name) const;          
    bool full_body_received() const;                             

private:
    std::unordered_map<std::string, UploadedFile> uploaded_files; 
    std::unordered_map<std::string, std::string> form_fields;     
    std::string method{};                                       
    std::string path{};                                         
    std::string http_version{};                                 
    std::string body{};                                         
    std::string token{};                                        
    std::string boundaryString{};                               
    std::unordered_map<std::string, std::string> header{};      
    std::string unparsedQuery{};                                
    std::unordered_map<std::string, std::string> queryParams{}; 
};

#endif // HTTP_REQUEST_H