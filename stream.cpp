#include <netinet/in.h> 
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <unordered_map>
#include <bits/stdc++.h>
#include <deque>
#include <tuple>
#include <map>
#include <thread>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <functional>
#include <thread>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include "auth.h"
#include "filter.h"
#include "groupby.h"

#define DATA_BUFFER 16384

int close_socket(int fd) {
    close(fd);
    return -1;
}

long get_time_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int parse_row(const std::string row, const std::vector<std::string> colnames, const str_str_umap col_dtype_map, csv_rows &data, int *&index, const FilterTree *tree, const GroupByInp grp, GroupByCache &grpby_cache) 
{
    
    if (index[0] < 19) 
    {
        index[0] += 1;
        return -1;
    }

    int p = 0;
    int k = 0;
    
    std::string curr = "";
    str_str_umap col_vals;

    for (std::string col : colnames) 
    {
        col_vals[col] = "";
    }

    for (int i = 0; i < row.size()-1; i++) 
    {
        if ((row[i] == '"') || (row[i] == '\'')) p++;
        else if ((row[i] == ',') && (p % 2 == 0)) 
        {
            std::string col = colnames[k++];
            col_vals[col] = curr;

            curr = "";
        }
        else 
        {
            curr += row[i];
        }
    }

    if (curr.size() > 0) 
    {
        std::string col = colnames[k++];
        col_vals[col] = curr;
    }
    
    index[0] += 1;
    bool is_row_valid = true;

    if (tree != nullptr) 
    {
        is_row_valid = filter(tree, col_vals, col_dtype_map);
    }

    if (is_row_valid && grp.group_by_columns.size() > 0) 
    {
        update_key_values(grp, grpby_cache, col_vals, col_dtype_map);
    }

    else if (is_row_valid) 
    {
        data.push_back(col_vals);
    }

    else 
    {
        return -1;
    }

    return 1;
}

std::vector<std::string> split_inline(std::string &s, const std::string delimiter) 
{
    size_t pos = 0;
    std::vector<std::string> parts;

    while ((pos = s.find(delimiter)) != std::string::npos) 
    {
        std::string token = s.substr(0, pos);
        if (token.size() > 0) parts.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    
    return parts;
}

std::vector<std::string> split(std::string s, const std::string delimiter) 
{
    size_t pos = 0;
    std::vector<std::string> parts;

    while ((pos = s.find(delimiter)) != std::string::npos) 
    {
        std::string token = s.substr(0, pos);
        if (token.size() > 0) parts.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    parts.push_back(s);
    
    return parts;
}

int connect(const std::string host, const int port, bool blocking=false) 
{
    struct sockaddr_in saddr;
    int fd, ret_val, ret;
    struct hostent *local_host;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 

    if (fd == -1) 
    {
        perror ("Creating socket failed");
        return -1;
    }

    printf("Created a socket with fd: %d\n", fd);

    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(port);     
    local_host = gethostbyname(host.c_str());
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    if (!blocking) fcntl(fd, F_SETFL, O_NONBLOCK);

    long timeout = get_time_ms() + 2;

    while (1) 
    {
        ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

        if (ret_val < 0) 
        {
            std::cout << "Connect to socket failed" << std::endl;
            if (get_time_ms() > timeout) 
            {
                std::cout << "Timeout...." << std::endl;
                close(fd);
                return -1;
            }
        }
        else 
        {
            break;
        }
    }

    printf("The Socket is now connected\n");
    return fd;
}

struct SSLdata 
{
    SSL *conn;
    int err;
};

SSLdata get_ssl_conn(const int fd) 
{
    SSL_load_error_strings ();
    SSL_library_init ();
    SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    SSLdata mydata;

    // create an SSL connection and attach it to the socket
    SSL *conn = SSL_new(ssl_ctx);
    SSL_set_fd(conn, fd);

    mydata.conn = conn;

    int err = SSL_connect(conn);
    mydata.err = 1;

    if (err != 1) mydata.err = -1;

    return mydata;
} 

int send_to_socket_ssl(SSL *conn, const std::string msg) 
{
    const char *msg_chr = msg.c_str();
    int res = SSL_write(conn, msg_chr, strlen(msg_chr));
    return res;
}

int recv_from_socket_ssl(SSL *conn, std::vector<std::string> &msgs, const std::string sep, const std::vector<std::string> colnames, const str_str_umap col_dtype_map, csv_rows &data, int *&index, const FilterTree *tree, const GroupByInp grp, GroupByCache &grpby_cache) 
{
    std::string remainder = "";

    while (1) 
    {
        char *buf = new char[DATA_BUFFER];
        int ret_data = SSL_read(conn, buf, DATA_BUFFER);

        if (ret_data > 0) 
        {
            std::string msg(buf, buf + ret_data);
            msg = remainder + msg;

            std::vector<std::string> parts = split_inline(msg, sep);

            for (std::string row : parts)
            {
                parse_row(row, colnames, col_dtype_map, data, index, tree, grp, grpby_cache);
            }
            
            msgs.insert(msgs.end(), parts.begin(), parts.end());
            remainder = msg;
        } 
        else if (ret_data == 0) 
        {
            break;
        }
        else 
        {
            fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
            return -1;
        }
        

    }

    return 1;
}

int colnames_dtypes(const std::string col_dtypes, std::vector<std::string> &colnames, str_str_umap &col_dtype_map) 
{
    std::vector<std::string> cols = split(col_dtypes, ",");

    for (std::string s : cols) 
    {
        std::vector<std::string> col_dtype = split(s, ":");

        if (col_dtype.size() == 2) 
        {
            std::string colname = col_dtype[0];
            std::string dtype = col_dtype[1];

            colnames.push_back(colname);
            col_dtype_map[colname] = dtype;
        }
        else 
        {
            return -1;
        }
    }

    return 1;
}

int query_process(const std::string query, FilterTree *&mytree, GroupByInp &grpby) 
{
    std::vector<std::string> stages = split(query, "|");

    for (std::string stage :  stages) 
    {
        std::vector<std::string> stage_parts = split(stage, ":");

        if (stage_parts.size() == 2) 
        {
            if (stage_parts[0] == "filter") 
            {
                std::string filter_query = stage_parts[1];
                filter_query = replace_multiple_spaces(filter_query);
                filter_query = replace_space_before_parenthesis(filter_query);
                mytree = build_filter_tree(filter_query);
            }

            else if (stage_parts[0] == "groupby") 
            {
                std::vector<std::string> column_values = split(stage_parts[1], "&");

                for (std::string colval : column_values) 
                {
                    std::vector<std::string> colval_split = split(colval, "=");
                    
                    if (colval_split.size() == 2) 
                    {
                        std::string attr = colval_split[0];
                        std::string vals = colval_split[1];

                        std::vector<std::string> split_vals = split(vals, ",");

                        if (attr == "metric") 
                        {
                            for (std::string metric : split_vals) grpby.metric_names.push_back(metric);
                        }

                        else if (attr == "on") 
                        {
                            for (std::string oncol : split_vals) grpby.group_by_columns.push_back(oncol);
                        }

                        else if (attr == "summarize") 
                        {
                            for (std::string scol : split_vals) grpby.summarize_cols.push_back(scol);
                        }

                        else if (attr == "summarize_new") 
                        {
                            for (std::string scol_new : split_vals) grpby.summarize_cols_new.push_back(scol_new);
                        }

                        else 
                        {
                            return -1;
                        }
                    }

                    else 
                    {
                        return -1;
                    }
                }
            }

            else 
            {
                return -1;
            }
        }

        else 
        {
            return -1;
        }
    }

    return 1;
}

int main(int argc, char *argv[]) {
    std::string storage_account = std::string(argv[1]);
    std::string path = std::string(argv[2]);
    std::string access_key = std::string(argv[3]);
    std::string host = std::string(argv[4]);
    std::string col_dtypes = std::string(argv[5]);
    std::string filter_groupby = std::string(argv[6]);

    GetRequest auth = get_auth_signature(storage_account, path, access_key);

    std::vector<std::string> colnames;

    str_str_umap col_dtype_map;
    csv_rows data;

    int res = colnames_dtypes(col_dtypes, colnames, col_dtype_map);
    if (res == -1) 
    {
        std::cout << "Error parsing column dtype values" << std::endl;
    }

    FilterTree *mytree = nullptr;
    GroupByInp grpby = {};
    GroupByCache grpby_cache;

    res = query_process(filter_groupby, mytree, grpby);

    if (res == -1) 
    {
        std::cout << "Error parsing filter group by query" << std::endl;
    }
    
    int port = 443;

    int fd = connect(host, port, true);
    SSLdata conn_data = get_ssl_conn(fd);

    if (conn_data.err == -1) 
    {
        std::cout << "SSL connection failed...." << std::endl;
        close_socket(fd);
    }

    SSL *conn = conn_data.conn;
    
    std::string msg = "GET " + path + " HTTP/1.1\r\nHost: " + host + "\r\nx-ms-date: " + auth.request_date + "\r\nx-ms-version: " + auth.storage_service_version + "\r\nAuthorization: " + auth.authorization_header + "\r\nConnection: close\r\n\r\n";
    std::cout << msg << std::endl;

    int send_res = send_to_socket_ssl(conn, msg);

    if (send_res < 0) 
    {
        std::cout << "failed to send request..." << std::endl;
        close_socket(fd);
        return 0;
    }

    std::vector<std::string> msgs;
    int *index = new int[1];
    index[0] = -1;
    int recv_res = recv_from_socket_ssl(conn, msgs, "\n", colnames, col_dtype_map, data, index, mytree, grpby, grpby_cache);

    if (recv_res < 0) 
    {
        std::cout << "failed to receive data..." << std::endl;
        close_socket(fd);
        return 0;
    }
    
    if (grpby_cache.grpby_key_values.size() > 0) 
    {
        data.clear();

        for (auto &kv : grpby_cache.grpby_key_values) 
        {
            str_str_umap col_vals;

            std::string grp_col_key = kv.first;
            std::unordered_map<std::string, ColumnTypes> &summarize_values = kv.second;

            std::vector<std::string> cols = split(grp_col_key, "|");

            for (int j = 0; j < cols.size(); j++) 
            {
                std::string colname = grpby.group_by_columns[j];
                col_vals[colname] = cols[j];
            }

            for (int j = 0; j < grpby.summarize_cols.size(); j++)
            {
                std::string o_col = grpby.summarize_cols[j];
                std::string u_col = grpby.summarize_cols_new[j];

                std::string dtype;

                if (o_col == "_count")
                {
                    dtype = "int";
                }
                else
                {
                    dtype = col_dtype_map[o_col];
                }

                if (dtype == "int") col_vals[u_col] = std::to_string(summarize_values[u_col].int_val);
                else if (dtype == "float" || dtype == "double") col_vals[u_col] = std::to_string(summarize_values[u_col].double_val);
                else col_vals[u_col] = "NULL";
            }

            data.push_back(col_vals);
        }
    }

    for (int i = 0; i < data.size(); i++) 
    {
        for (auto &kv : data[i]) 
        {
            std::cout << kv.first << " : " << kv.second << ", ";
        }
        std::cout << std::endl;
    }

    close_socket(fd);
    return 0;
}