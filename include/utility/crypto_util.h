#pragma once
#include <string>

class CryptoUtil {
public:
    static std::string hash_password(const std::string& password);
    static std::string generate_session_token(const std::string& user_id);
    static bool verify_token(const std::string& token);
};
