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


const int partlinecount = 200000;
const int lineitemlinecount = 6001215;
// std::unordered_map<int, int> hashtable;
tbb::concurrent_unordered_map<int, int> hashtable;

struct stat sb;
std::vector<int> p_partkeys, l_quantities, p_sizes, l_partkeys;
std::vector<std::string> p_brands, p_containers, l_shipmodes, l_shipinstructs;
std::vector<double> l_discounts, l_extendedprices;


void setup() {
    

    int i = 0;
    std::string line;
    int fptr = open("/Users/shiro/tpch-dbgen/part.tbl", O_RDONLY);
    if (fstat(fptr,&sb) == -1) {
        perror("Couldnt get file size\n");
    }

    char *file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);


    int counter = 0;
    int linecount = 0;
    std::string p_partkey, p_size, p_brand, p_container = "";
    while (linecount < partlinecount) {
        if (file_in_memory[i] == '\n') {
            p_partkeys.push_back(std::stoi(p_partkey));
            p_sizes.push_back(std::stoi(p_size));
            p_brands.push_back(p_brand);
            p_containers.push_back(p_container);
            p_partkey = "";
            p_size = "";
            p_brand = "";
            p_container = "";
            counter = 0;
            linecount++;
        }
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 0) {
            p_partkey = p_partkey + file_in_memory[i];
        } else if (counter == 3) {
            p_brand = p_brand + file_in_memory[i];
        } else if (counter == 5) {
            p_size = p_size + file_in_memory[i];
        } else if (counter == 6) {
            p_container = p_container + file_in_memory[i];
        }
        i++;
    }
    linecount = 0;
    int c = 0;
    i = 0;
    close(fptr);
    munmap(file_in_memory, sb.st_size);
    fptr = open("/Users/shiro/tpch-dbgen/lineitem.tbl", O_RDONLY);
    fstat(fptr,&sb);
    file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);
    std::string l_discount, l_extendedprice, l_quantity, l_shipinstruct, l_shipmode, l_partkey= "";
    counter = 0;


    while (linecount < lineitemlinecount) {

        if (file_in_memory[i] == '\n') {

            l_partkeys.push_back(std::stoi(l_partkey));
            l_discounts.push_back(std::stod(l_discount));
            l_extendedprices.push_back(std::stod(l_extendedprice));
            l_quantities.push_back(std::stoi(l_quantity));
            l_shipinstructs.push_back(l_shipinstruct);
            l_shipmodes.push_back(l_shipmode);
            l_partkey = "";
            l_discount = "";
            l_extendedprice = "";
            l_quantity = "";
            l_shipinstruct = "";
            l_shipmode = "";
            counter = 0;
            linecount++;
        }
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 1) {
            l_partkey = l_partkey + file_in_memory[i];
        } else if (counter == 4) {
            l_quantity = l_quantity + file_in_memory[i];
        } else if (counter == 5) {
            l_extendedprice = l_extendedprice + file_in_memory[i];
        } else if (counter == 6) {
            l_discount = l_discount + file_in_memory[i];
        } else if (counter == 13) {
            l_shipinstruct = l_shipinstruct + file_in_memory[i];
        } else if (counter == 14) {
            l_shipmode = l_shipmode + file_in_memory[i];
        }
        i++;
    }
    
    
    close(fptr);
    munmap(file_in_memory, sb.st_size);
}
void test() {
       //faster to check partkey first or shipinstruct and shipmode conditions? 
    // first shipmodes and instructs -> 145ms avg.  and with -O3 flag -> 103ms avg.
    // first hashtable check -> 410ms avg. and with -O3 flag -> 90ms avg.
    double revenue = 0;
    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for shared(hashtable)
    for (int i = 0; i < p_partkeys.size(); i++) {
        if (p_brands[i] == "Brand#12" && (p_containers[i] == "SM CASE" || p_containers[i] == "SM BOX" || p_containers[i] == "SM PACK" || p_containers[i] == "SM PKG")
        && (p_sizes[i] >=1 && p_sizes[i] <= 5)) {
            hashtable[p_partkeys[i]] = 12;
        } else if (p_brands[i] == "Brand#23" && (p_containers[i] == "MED BAG" || p_containers[i] == "MED BOX" || p_containers[i] == "MED PACK" || p_containers[i] == "MED PKG")
        && (p_sizes[i] >=1 && p_sizes[i] <= 10)) {
            hashtable[p_partkeys[i]] = 23;
        } else if (p_brands[i] == "Brand#34" && (p_containers[i] == "LG CASE" || p_containers[i] == "LG BOX" || p_containers[i] == "LG PACK" || p_containers[i] == "LG PKG")
        && (p_sizes[i] >=1 && p_sizes[i] <= 15)) {
            hashtable[p_partkeys[i]] = 34;
        }
    }
    #pragma omp parallel for shared(revenue, l_shipmodes, l_shipinstructs, l_partkeys, hashtable)
    for (int i = 0; i < l_partkeys.size(); i++) {

            if ((l_shipmodes[i] == "AIR" || l_shipmodes[i] == "AIR REG") && (l_shipinstructs[i] == "DELIVER IN PERSON")) {
                if (hashtable.count(l_partkeys[i])) {
                
                switch(hashtable[l_partkeys[i]]) {
                    case 12:
                        if (l_quantities[i] >= 1 && l_quantities[i] <= 11) {
                            #pragma omp critical
                            revenue += l_extendedprices[i] * (1-l_discounts[i]);
                        }
                        break;
                    case 23:
                        if (l_quantities[i] >= 10 && l_quantities[i] <= 20) {
                            #pragma omp critical
                            revenue += l_extendedprices[i] * (1-l_discounts[i]);
                        }
                        break;
                    case 34:
                        if (l_quantities[i] >= 20 && l_quantities[i] <= 30) {
                            #pragma omp critical
                            revenue += l_extendedprices[i] * (1-l_discounts[i]);
                        }
                        break;
                }
            }
        }
    }
    
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    #pragma omp barrier
    std::cout << "revenue = " << std::fixed << revenue << std::endl;
    std::cout << "duration = " << duration.count() << std::endl;
}
int main() {
setup(); 

omp_set_dynamic(0);
omp_set_num_threads(1);
test();
omp_set_num_threads(2);
test();
omp_set_num_threads(4);
test();
omp_set_num_threads(8);
test();



}