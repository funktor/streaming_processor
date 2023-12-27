#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "datastructure.h"

struct FilterTree {
    std::string op;
    std::string column;
    std::string value;
    std::string comparator;
    FilterTree *left;
    FilterTree *right;
};

void replace_all_substrings(std::string& source_string, const std::string source_pattern, const std::string target_pattern);
std::string replace_multiple_spaces(std::string query);
std::string replace_space_before_parenthesis(std::string query);
bool filter(const FilterTree *tree, const str_str_umap row, const str_str_umap dtypes);
FilterTree* build_filter_tree(std::string query);

