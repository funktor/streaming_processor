#include <string>
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <cctype>
#include <string_view>
#include <array>

struct GetRequest {
    std::string request_date;
    std::string storage_service_version;
    std::string authorization_header;
};

std::string CalcHmacSHA256(std::string_view decodedKey, std::string_view msg);
std::string get_gmt_datetime();
std::string string_to_hex(const std::string& input);
GetRequest get_auth_signature(std::string storage_account, std::string file_path, std::string access_key);