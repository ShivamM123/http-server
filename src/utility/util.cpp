#include "utility/util.h" // Adjust include path based on your setup
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

namespace Util {
    string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }

    string to_lower(const string& str) {
        string result = str;
        for (char& c : result) c = tolower(c);
        return result;
    }

    string to_upper(const string& str) {
        string result = str;
        for (char& c : result) c = toupper(c);
        return result;
    }

    vector<string> split_string(const string& str, char delimiter) {
        vector<string> tokens;
        stringstream ss(str);
        string token;
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool is_digit(const string& str) {
        return !str.empty() && all_of(str.begin(), str.end(), ::isdigit);
    }

    bool has_upper_case(const string& password) {
        return any_of(password.begin(), password.end(), ::isupper);
    }

    bool has_lower_case(const string& password) {
        return any_of(password.begin(), password.end(), ::islower);
    }

    bool has_digit(const string& password) {
        return any_of(password.begin(), password.end(), ::isdigit);
    }

    bool is_username_valid(const string& username) {
        return !username.empty() && username.size() <= 32 &&
               all_of(username.begin(), username.end(), [](char c) {
                   return isalnum(c) || c == '_' || c == '-';
               });
    }

    string char_to_hex(char c) {
        ostringstream hex_stream;
        hex_stream << hex << uppercase << setw(2) << setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
        return hex_stream.str();
    }

    string url_encode(const string& message) {
        ostringstream encoded;
        for (auto& v : message) {
            if (isalnum(v) || v == '-' || v == '_' || v == '~' || v == '.') {
                encoded << v;
            } else {
                encoded << '%' << char_to_hex(v);
            }
        }
        return encoded.str();
    }

    string url_decode(const string& message) {
        ostringstream decoded;
        for (size_t i = 0; i < message.size(); ++i) {
            char v = message[i];
            if (isalnum(v) || v == '-' || v == '_' || v == '.') {
                decoded << v;
            } else if (v == '+') {
                decoded << ' ';
            } else if (v == '%' && i + 2 < message.size()) {
                // Decode %XY to ASCII
                decoded << static_cast<char>(stoi(message.substr(i + 1, 2), nullptr, 16));
                i += 2;
            }
        }
        return decoded.str();
    }

    void make_error_response(int code, const string& message, HttpResponse& res) {
        res.set_status(code, message);
        res.set_header("Content-Type", "application/json");
        res.set_body("{\"error\": \"" + message + "\"}");
    }
}