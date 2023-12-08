#include "filter.h"

bool filter(FilterTree *tree, 
            std::unordered_map<std::string, std::string> row, 
            std::unordered_map<std::string, std::string> dtypes) {

    if (tree == nullptr) {
        return false;
    }

    else if (tree->left == nullptr && tree->right == nullptr) {
        std::string cmp = tree->comparator;
        std::string col = tree->column;
        std::string val = tree->value;

        if (cmp == "==") return row[col] == val;
        else if (cmp == "!=") return row[col] != val;
        else if (cmp == ">=") {
            if (dtypes[col] == "int" || dtypes[col] == "float") {
                return std::stof(row[col]) >= std::stof(val);
            }
            return false;
        }
        else if (cmp == "<=") {
            if (dtypes[col] == "int" || dtypes[col] == "float") {
                return std::stof(row[col]) <= std::stof(val);
            }
            return false;
        }
        else if (cmp == ">") {
            if (dtypes[col] == "int" || dtypes[col] == "float") {
                return std::stof(row[col]) > std::stof(val);
            }
            return false;
        }
        else if (cmp == "<") {
            if (dtypes[col] == "int" || dtypes[col] == "float") {
                return std::stof(row[col]) < std::stof(val);
            }
            return false;
        }
        return false;
    }

    else {
        bool lt_d = filter(tree->left, row, dtypes);
        bool rt_d = filter(tree->right, row, dtypes);

        if (tree->op == "and") {
            return lt_d && rt_d;
        }

        return lt_d || rt_d;
    }
}

FilterTree* build_filter_tree(std::string query) {
    FilterTree *mytree = new FilterTree{"", "", "", "", nullptr, nullptr};

    int p = 0;
    int q = 0;
    std::string curr_st = "";
    std::string curr_wd = "";
    
    for (int i = 0; i < query.size(); i++) {        
        if (query[i] == '(') {
            p += 1;
            curr_wd += query[i];
        }
            
        else if (query[i] == ')') {
            p -= 1;
            curr_wd += query[i];
        }
                
        else if (query[i] != ' ') {
            if (query[i] == '\'' || query[i] == '"') {
                q += 1;
            }  
            curr_wd += query[i];
        }
            
        else if (query[i] == ' ') {
            if (q % 2 == 1) {
                curr_wd += query[i];
            }
                
            else {
                if ((curr_wd == "and" || curr_wd == "or") && p == 0 && mytree->left == nullptr) {
                    std::cout << curr_st << std::endl;

                    mytree->op = curr_wd;
                    
                    if (curr_st[0] == '(' && *curr_st.rbegin() == ')') {
                        std::string curr_st_new = curr_st.substr(1, curr_st.size()-2);
                        mytree->left = build_filter_tree(curr_st_new);
                    }
                    else {
                        mytree->left = build_filter_tree(curr_st);
                    }
                        
                    curr_st = "";
                }
                else {
                    if (curr_st.size() > 0) curr_st = curr_st + " " + curr_wd;
                    else curr_st = curr_wd;
                }

                curr_wd = "";
            }
        }
    }

    if (curr_st.size() > 0) curr_st = curr_st + " " + curr_wd;
    else curr_st = curr_wd;
    
    if (p == 0 && mytree->left != nullptr) {
        std::cout << curr_st << std::endl;

        if (curr_st[0] == '(' && *curr_st.rbegin() == ')') {
            std::string curr_st_new = curr_st.substr(1, curr_st.size()-2);
            mytree->right = build_filter_tree(curr_st_new);
        }
        else {
            mytree->right = build_filter_tree(curr_st);
        }
    }

    if (mytree->left == nullptr && mytree->right == nullptr) {
        if (query[0] == '(' && *query.rbegin() == ')') {
            std::string query_new = query.substr(1, query.size()-2);
            mytree = build_filter_tree(query_new);
        }

        else {
            q = 0;
            curr_wd = "";
            
            for (int i = 0; i < query.size(); i++) {
                if (query[i] != ' ' && query[i] != '(' && query[i] != ')') {
                    if (query[i] == '"' || query[i] == '\'') {
                        q += 1;
                    }
                    curr_wd += query[i];
                }
                    
                else if (query[i] == ' ') {
                    if (q % 2 == 1) {
                        curr_wd += query[i];
                    }

                    else {
                        if (mytree->column == "") {
                            mytree->column = curr_wd;
                        }
                        else if (mytree->comparator == "") {
                            mytree->comparator = curr_wd;
                        }
                        else {
                            mytree->value = curr_wd;

                            bool a = mytree->value[0] == '\'' && *(mytree->value).rbegin() == '\'';
                            bool b = mytree->value[0] == '"' && *(mytree->value).rbegin() == '"';

                            if (a || b) {
                                mytree->value = mytree->value.substr(1, mytree->value.size()-2);
                            }
                        }
        
                        curr_wd = "";
                    }
                }
            }
            
            if (mytree->column == "") {
                mytree->column = curr_wd;
            }
            else if (mytree->comparator == "") {
                mytree->comparator = curr_wd;
            }
            else {
                mytree->value = curr_wd;

                bool a = mytree->value[0] == '\'' && *(mytree->value).rbegin() == '\'';
                bool b = mytree->value[0] == '"' && *(mytree->value).rbegin() == '"';

                if (a || b) {
                    mytree->value = mytree->value.substr(1, mytree->value.size()-2);
                }
            }
        }
    }

    return mytree;
}