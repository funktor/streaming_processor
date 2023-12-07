#include <string>
#include <iostream>
#include <unordered_map>

struct FilterTree {
    std::string op;
    std::string column;
    std::string value;
    std::string comparator;
    FilterTree *left;
    FilterTree *right;
};

bool filter(FilterTree *tree, std::unordered_map<std::string, std::string> row, std::unordered_map<std::string, std::string> dtypes);
FilterTree* build_filter_tree(std::string query);