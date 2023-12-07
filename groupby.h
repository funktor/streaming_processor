#include <string>
#include <vector>

struct GroupBy {
    std::vector<std::string> group_by_columns;
    std::string summarize_col;
    std::string summarize_col_new;
    std::string metric_name;
};