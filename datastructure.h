#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>

struct ColumnTypes 
{
    std::string str_val="";
    long int_val=0;
    double double_val=0.0;
    bool binary_val=false;
    tm *datetime_val=nullptr;
};

typedef std::unordered_map<std::string, std::string> str_str_umap;
typedef std::unordered_map<std::string, long> str_long_umap;
typedef std::unordered_map<std::string, int> str_int_umap;
typedef std::unordered_map<std::string, double> str_double_umap;
typedef std::unordered_map<std::string, std::unordered_map<std::string, ColumnTypes>> grpby_umap;
typedef std::vector<std::unordered_map<std::string, std::string>> csv_rows;

struct GroupByInp 
{
    std::vector<std::string> group_by_columns;
    std::vector<std::string> summarize_cols;
    std::vector<std::string> summarize_cols_new;
    std::vector<std::string> metric_names;
};

struct GroupByCache
{
    grpby_umap grpby_key_values;
    str_long_umap grpby_cnts;
};