# streaming_processor
Stream Processor in C++

## Library Installation (Ubuntu 22.04)
1. Install gcc-11
2. Install openssl and libssl-dev: sudo apt-get update & sudo apt install openssl libssl-dev
3. Install boost library if not installed: Follow steps outlined here - https://linux.how2shout.com/how-to-install-boost-c-on-ubuntu-20-04-or-22-04/

## Steps to compile and run
1. Compile using:
```
g++ -O3 *.cpp -o stream -lssl -lcrypto
```

2. For accessing Azure blob storage, we need to pass the storage account name, the path of the file inside and the access key (obtained from the azure portal). Here we need to first set these environment variables through command line:
```
storage_account="spotpricingdevst"
path="/container_name/path_to_file.csv"
access_key=<storage_account_access_key>
```

3. Run the compiled program with similar arguments:
```
./stream "$storage_account" "$path" "$access_key" "$storage_account.blob.core.windows.net" "col_A:string,col_B:int,col_C:float" "filter:col_B == x and col_C >= y|groupby:metric=sum,avg,count&on=col_A&summarize=col_B,col_C,_count&summarize_new=sum_col_B,avg_col_C,counts"
```
