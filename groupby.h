#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <unordered_map>
#include "datastructure.h"

bool is_number(const std::string s);
void update_value_int(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache);
void update_value_dbl(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache);
void update_key_values(GroupByInp grp_inp, GroupByCache &grpby_cache, str_str_umap col_vals, str_str_umap col_dtype_map);