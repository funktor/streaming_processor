#include <string>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>

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
bool filter(FilterTree *tree, std::unordered_map<std::string, std::string> row, std::unordered_map<std::string, std::string> dtypes);
FilterTree* build_filter_tree(std::string query);