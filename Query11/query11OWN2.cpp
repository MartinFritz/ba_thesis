#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <map>
#include <algorithm>
#include <omp.h>
#include <tbb/concurrent_unordered_map.h>
#include <execution>


bool comp(std::pair<int,long double> p1, std::pair<int,long double> p2) {
    
    return (p1.second > p2.second);
}

int main() {
    int fptr = open("/Users/shiro/tpch-dbgen/nation.tbl", O_RDONLY);
    struct stat sb;
   // struct stat sc;
   // struct stat sd;
    const int linecountnation = 25;
    const int linecountpart = 800000;
    const int linecountsupplier = 10000;
    std::string s_suppkey = "";
    std::string ps_suppkey = "";
    std::string n_nationkey = "";
    std::string s_nationkey = "";
    std::string n_name = "";
    std::string ps_supplycost = "";
    std::string ps_availqty = "";
    std::string ps_partkey = "";
    std::vector<int> s_suppkeys, ps_partkeys, ps_suppkeys, n_nationkeys, s_nationkeys, ps_availqties;
    std::vector<std::string> n_names;
    std::vector<double> ps_supplycosts;
tbb::concurrent_unordered_map<int,std::string> map;
tbb::concurrent_unordered_map<int,std::string> nextmap;
tbb::concurrent_unordered_map<int,double> resmap;
    std::unordered_map<int,double> result;
    int c;
    if (fstat(fptr,&sb) == -1) {
        perror("Couldnt get file size\n");
    }

    char *file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);

    int size = 0;
    int counter = 0;
    int linecount = 0;
    int i = 0;

    while (linecount < linecountnation) {
        if (file_in_memory[i] == '\n') {
            n_names.push_back(n_name);
            n_nationkeys.push_back(std::stoi(n_nationkey));
            n_name = "";
            n_nationkey = "";
            counter = 0;
            linecount++;
        }
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 0) {
            n_nationkey = n_nationkey + file_in_memory[i];
        } else if (counter == 1) {
            n_name = n_name + file_in_memory[i];
        }
        i++;
    }
    munmap(file_in_memory,sb.st_size);
    close(fptr);
    
    fptr = open("/Users/shiro/tpch-dbgen/partsupp.tbl",O_RDONLY);
    
    if (fstat(fptr,&sb) == -1) {
        perror("Couldnt get file size\n");
    }

    file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);
    
    i = 0;
    counter = 0;
    linecount = 0;
    while (linecount < linecountpart) {
        if (file_in_memory[i] == '\n') {
            ps_partkeys.push_back(std::stoi(ps_partkey));
            ps_suppkeys.push_back(std::stoi(ps_suppkey));
            ps_availqties.push_back(std::stoi(ps_availqty));
            ps_supplycosts.push_back(std::stod(ps_supplycost));
            ps_partkey = "";
            ps_suppkey = "";
            ps_availqty = "";
            ps_supplycost = "";
            counter = 0;
            linecount++;
        }
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 0) {
            ps_partkey = ps_partkey + file_in_memory[i];
        } else if (counter == 1) {
            ps_suppkey = ps_suppkey + file_in_memory[i];
        } else if (counter == 2) {
            ps_availqty = ps_availqty + file_in_memory[i];
        } else if (counter == 3) {
            ps_supplycost = ps_supplycost + file_in_memory[i];
        }
        i++;
    }
    munmap(file_in_memory,sb.st_size);
    close(fptr);
    fptr = open("/Users/shiro/tpch-dbgen/supplier.tbl",O_RDONLY);
    
    if (fstat(fptr,&sb) == -1) {
        perror("Couldnt get file size\n");
    }

    file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);
    
    i = 0;
    counter = 0;
    linecount = 0;

    while (linecount < linecountsupplier) {

        if (file_in_memory[i] == '\n') {
            s_nationkeys.push_back(std::stoi(s_nationkey));
            s_suppkeys.push_back(std::stoi(s_suppkey));
            s_nationkey = "";
            s_suppkey = "";
            counter = 0;
            linecount++;
        } 
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 0) {
            s_suppkey = s_suppkey + file_in_memory[i];
        } else if (counter == 3) {
            s_nationkey = s_nationkey + file_in_memory[i];
        }
        i++;
    }
    munmap(file_in_memory,sb.st_size);
    close(fptr);
    std::vector<int> mapkeys;
    omp_set_dynamic(0);
    omp_set_num_threads(4);

    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for shared(map)
    for (int i = 0; i < n_names.size(); i++) {
        if (n_names[i] == "GERMANY") { 
            map[n_nationkeys[i]] = "GERMANY";
        }
    }
    #pragma omp parallel for shared(map,nextmap)
    for (int i = 0; i < s_nationkeys.size(); i++) {
        if (map.count(s_nationkeys[i])) {
            nextmap[s_suppkeys[i]] = s_nationkeys[i];
        }
    }
    long double overallvalue = 0;
    #pragma omp parallel for shared(nextmap,resmap,overallvalue,mapkeys, ps_supplycosts, ps_availqties)
    for (int i = 0; i < ps_suppkeys.size(); i++) {
        if (nextmap.count(ps_suppkeys[i])) {
            #pragma omp critical
            overallvalue += (ps_supplycosts[i]*ps_availqties[i]);
            #pragma omp critical 
            {
            if (resmap.count(ps_partkeys[i])) {
                resmap[ps_partkeys[i]] += (ps_supplycosts[i]*ps_availqties[i]); 
            } else {
                resmap[ps_partkeys[i]] = (ps_supplycosts[i]*ps_availqties[i]); 
                mapkeys.push_back(ps_partkeys[i]);
            }         
            }

        }
    }
    #pragma omp parallel for shared(overallvalue, result, mapkeys) 
    for (int i = 0; i < mapkeys.size(); i++) {
        auto x = resmap[mapkeys[i]];
        if (x > 0.0001*overallvalue) {
            if (result.count(mapkeys[i])) {
                #pragma omp critical
                result[mapkeys[i]] += x;
            } else {
                #pragma omp critical
                result[mapkeys[i]] = x;
            }
        }
    }
    
    std::vector<std::pair<int,double>>orderedpartsupps(result.begin(),result.end());
    std::sort(std::execution::par,orderedpartsupps.begin(), orderedpartsupps.end(), comp);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    std::cout << "result size = " << orderedpartsupps.size() << std::endl;
    std::cout << "duration = " << duration.count() << std::endl;
    std::cout << "overallvalue = " << std::fixed << overallvalue  << std::endl;
    
    return 0;
}