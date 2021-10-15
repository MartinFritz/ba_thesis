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

const int lineitemlinecount = 6001215;
struct stat sb;


int main() {

    int i = 0;

    int fptr = open("/Users/x/tpch-dbgen/lineitem.tbl", O_RDONLY);
    fstat(fptr,&sb);
    char *file_in_memory = (char *) mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fptr, 0);
    std::string l_discount, l_extendedprice, l_quantity, l_shipdate = "";
    std::vector<double> l_discounts;
    std::vector<double> l_extendedprices;
    std::vector<double> l_quantities;
    std::vector<std::string> l_shipdates;

    int counter = 0;
    int linecount = 0;

    while (linecount < lineitemlinecount) {

        if (file_in_memory[i] == '\n') {

            l_discounts.push_back(std::stod(l_discount));
            l_extendedprices.push_back(std::stod(l_extendedprice));
            l_quantities.push_back(std::stoi(l_quantity));
            l_shipdates.push_back(l_shipdate);
            l_discount = "";
            l_extendedprice = "";
            l_quantity = "";
            l_shipdate = "";
            counter = 0;
            linecount++;
        }
        if (file_in_memory[i] == '|') {
            counter++;
        } else if (counter == 4) {
            l_quantity = l_quantity + file_in_memory[i];
        } else if (counter == 5) {
            l_extendedprice = l_extendedprice + file_in_memory[i];
        } else if (counter == 6) {
            l_discount = l_discount + file_in_memory[i];
        } else if (counter == 10) {
            l_shipdate = l_shipdate + file_in_memory[i];
        } 
        i++;
    }  
    close(fptr);
    munmap(file_in_memory, sb.st_size);
    long double revenue = 0;
    omp_set_dynamic(0);
    omp_set_num_threads(4);

    auto start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel for reduction(+:revenue)
    for (int i = 0; i < lineitemlinecount; i++) {
        if (l_shipdates[i] >= "1994-01-01" && l_shipdates[i] < "1995-01-01" && l_discounts[i] >= 0.05 && l_discounts[i] <= 0.07 && l_quantities[i] < 24) {
            revenue += l_extendedprices[i]* l_discounts[i];

        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "revenue = " << std::fixed << revenue << std::endl;
    std::cout << "duration = " << duration.count() << std::endl;

    return 0;
}
