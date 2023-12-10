#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <unordered_map>
#include "datastructure.h"

struct ColumnTypes 
{
    std::string str_val="";
    long int_val=0;
    double double_val=0.0;
    bool binary_val=false;
    tm *datetime_val=nullptr;
};

typedef std::unordered_map<std::string, std::unordered_map<std::string, ColumnTypes>> grpby_umap;

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

bool is_number(const std::string s);
void update_value_int(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache);
void update_value_dbl(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache);
void update_key_values(const GroupByInp grp_inp, GroupByCache &grpby_cache, const str_str_umap col_vals, const str_str_umap col_dtype_map);