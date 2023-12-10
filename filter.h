#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include "datastructure.h"

struct FilterTree {
    std::string op;
    std::string column;
    std::string value;
    std::string comparator;
    FilterTree *left;
    FilterTree *right;
};

std::string replace_multiple_spaces(std::string query);
std::string replace_space_before_parenthesis(std::string query);
bool filter(const FilterTree *tree, const str_str_umap row, const str_str_umap dtypes);
FilterTree* build_filter_tree(std::string query);