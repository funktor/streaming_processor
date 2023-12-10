#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>

typedef std::unordered_map<std::string, std::string> str_str_umap;
typedef std::unordered_map<std::string, long> str_long_umap;
typedef std::unordered_map<std::string, int> str_int_umap;
typedef std::unordered_map<std::string, double> str_double_umap;
typedef std::vector<std::unordered_map<std::string, std::string>> csv_rows;