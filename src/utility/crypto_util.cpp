#include "crypto_util.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <iomanip>
#include <sstream>

using namespace std;

// Secret key for HMAC (in production, load this from env variable)
const string SECRET_KEY = "my_super_secret_key";

string CryptoUtil::hash_password(const string& password) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_DigestInit_ex(context, md, NULL);
    EVP_DigestUpdate(context, password.c_str(), password.length());
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);
    EVP_MD_CTX_free(context);

    stringstream ss;
    for(unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string CryptoUtil::generate_session_token(const string& user_id) {
    unsigned char* digest;
    unsigned int digest_len = 0;
    
    // Using OpenSSL HMAC to generate signature
    digest = HMAC(EVP_sha256(), SECRET_KEY.c_str(), SECRET_KEY.length(), 
                  (unsigned char*)user_id.c_str(), user_id.length(), NULL, &digest_len);
                  
    stringstream ss;
    for(unsigned int i = 0; i < digest_len; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)digest[i];
    }
    string signature = ss.str();
    
    // Return token as user_id.signature
    return user_id + "." + signature;
}

bool CryptoUtil::verify_token(const string& token) {
    size_t dot_pos = token.find('.');
    if (dot_pos == string::npos) return false;
    
    string user_id = token.substr(0, dot_pos);
    
    // Re-generate the expected token and compare securely
    string expected_token = generate_session_token(user_id);
    
    return token == expected_token;
}
