#include "auth.h"
#include "base64.h"

std::string CalcHmacSHA256(std::string_view decodedKey, std::string_view msg) {
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
    unsigned int hashLen;

    HMAC(
        EVP_sha256(),
        decodedKey.data(),
        static_cast<int>(decodedKey.size()),
        reinterpret_cast<unsigned char const*>(msg.data()),
        static_cast<int>(msg.size()),
        hash.data(),
        &hashLen
    );

    return std::string{reinterpret_cast<char const*>(hash.data()), hashLen};
}

std::string get_gmt_datetime() {
    time_t rawtime;
    struct tm * ptm;

    time ( &rawtime );

    ptm = gmtime ( &rawtime );

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%a, %d %h %Y %H:%M:%S %Z", ptm);
    return std::string(buffer);
}

std::string string_to_hex(const std::string& input) {
    static const char hex_digits[] = "0123456789abcdef";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input) {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

GetRequest get_auth_signature(std::string storage_account, 
                        std::string file_path, std::string access_key) {

    GetRequest req;
    std::string request_date = get_gmt_datetime();
    std::string storage_service_version = "2017-11-09";
    std::string resource = "/" + storage_account + file_path;
    std::string headers = "x-ms-date:" + request_date + "\nx-ms-version:" + storage_service_version;
    std::string string_to_sign = "GET\n\n\n\n\n\n\n\n\n\n\n\n" + headers + "\n" + resource;
    std::string hex_key = base64_decode(access_key);
    
    std::string_view key_view{hex_key};
    std::string_view msg_view{string_to_sign};

    std::string signature = CalcHmacSHA256(key_view, msg_view);
    signature = base64_encode(signature);

    std::string authorization_header = "SharedKey " + storage_account + ":" + signature;
    return {request_date, storage_service_version, authorization_header};
}
