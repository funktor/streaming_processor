#include "groupby.h"

bool is_number(const std::string s) 
{
    if (s.size() == 0) return false;

    int pos = s.find(".");
    if (pos != std::string::npos && pos > 100) return false;

    int c = 0;
    
    for (int i = 0; i < s.size(); i++) 
    {
        if (!isdigit(s[i]) && s[i] != '-' && s[i] != '.') return false;

        if (s[i] == '-' && i > 0) return false;

        if (s[i] == '.') 
        {
            c++;
            if (c > 1) return false;
        }
    }

    return true;
}

void update_value_int(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache) 
{
    if (is_number(summarize_col_val)) 
    {
        long val = std::stol(summarize_col_val);
        long cnts = grpby_cache.grpby_cnts[grpby_col_key];
        
        grpby_umap key_values = grpby_cache.grpby_key_values;

        if (key_values.find(grpby_col_key) != key_values.end() 
                && key_values[grpby_col_key].find(summarize_col) != key_values[grpby_col_key].end()) 
        {

            long v = key_values[grpby_col_key][summarize_col].int_val;

            if (metric == "sum") val += v;
            else if (metric == "min") val = std::min(val, v);
            else if (metric == "max") val = std::max(val, v);
            else if (metric == "count") val += 1L;
            else if (metric == "avg") val = (val*(cnts-1) + v)/cnts;
        }

        else if (key_values.find(grpby_col_key) != key_values.end() 
                    && key_values[grpby_col_key].find(summarize_col) == key_values[grpby_col_key].end())
        {
            ColumnTypes col_type = {"", 0, 0.0, false, nullptr};
            grpby_cache.grpby_key_values[grpby_col_key][summarize_col] = col_type;
        }

        else
        {
            std::unordered_map<std::string, ColumnTypes> summarize_col_values;
            grpby_cache.grpby_key_values[grpby_col_key] = summarize_col_values;
        }

        grpby_cache.grpby_key_values[grpby_col_key][summarize_col].int_val = val;
    }
}

void update_value_dbl(const std::string grpby_col_key, const std::string summarize_col, const std::string summarize_col_val, const std::string metric, GroupByCache &grpby_cache) 
{
    if (is_number(summarize_col_val)) 
    {
        double val = std::stod(summarize_col_val);
        long cnts = grpby_cache.grpby_cnts[grpby_col_key];

        grpby_umap key_values = grpby_cache.grpby_key_values;

        if (key_values.find(grpby_col_key) != key_values.end() 
                && key_values[grpby_col_key].find(summarize_col) != key_values[grpby_col_key].end()) 
        {

            double v = key_values[grpby_col_key][summarize_col].double_val;

            if (metric == "sum") val += v;
            else if (metric == "min") val = std::min(val, v);
            else if (metric == "max") val = std::max(val, v);
            else if (metric == "count") val += 1L;
            else if (metric == "avg") val = (val*((double)cnts-1.0) + v)/(double)cnts;
        }

        else if (key_values.find(grpby_col_key) != key_values.end() 
                    && key_values[grpby_col_key].find(summarize_col) == key_values[grpby_col_key].end())
        {
            ColumnTypes col_type = {"", 0, 0.0, false, nullptr};
            grpby_cache.grpby_key_values[grpby_col_key][summarize_col] = col_type;
        }

        else
        {
            std::unordered_map<std::string, ColumnTypes> summarize_col_values;
            grpby_cache.grpby_key_values[grpby_col_key] = summarize_col_values;
        }

        grpby_cache.grpby_key_values[grpby_col_key][summarize_col].double_val = val;
    }    
}

void update_key_values(GroupByInp grp_inp, GroupByCache &grpby_cache, str_str_umap col_vals, str_str_umap col_dtype_map) 
{
    std::string col_key = "";
    std::vector<std::string> colnames = grp_inp.group_by_columns;

    for (int i = 0; i < colnames.size()-1; i++) 
    {
        col_key += col_vals[colnames[i]] + "|";
    }

    col_key += col_vals[*colnames.rbegin()];

    if (grpby_cache.grpby_cnts.find(col_key) == grpby_cache.grpby_cnts.end()) 
    {
        grpby_cache.grpby_cnts[col_key] = 0;
    }

    grpby_cache.grpby_cnts[col_key] += 1;

    for (int i = 0; i < grp_inp.metric_names.size(); i++) 
    {

        std::string metric = grp_inp.metric_names[i];
        std::string summarize_col_new = grp_inp.summarize_cols_new[i];
        std::string summarize_col = grp_inp.summarize_cols[i];

        std::string col_val, dtype;

        if (summarize_col == "_count")
        {
            col_val = "1";
            dtype = "int";
        }
        else
        {
            col_val = col_vals[summarize_col];
            dtype = col_dtype_map[summarize_col];
        }

        if (dtype == "int") update_value_int(col_key, summarize_col_new, col_val, metric, grpby_cache);
        else if (dtype == "float" || dtype == "double") update_value_dbl(col_key, summarize_col_new, col_val, metric, grpby_cache);
    }
}