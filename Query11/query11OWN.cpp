#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>

class PartSupp {

    public:
    int partkey;
    int suppkey;
    int availqty;
    long double supplycost;
    long double value;

    PartSupp(std::string l) {
        int i = 0;
        int count = 0;
        std::string tmp = "";
        while(l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }
        i++;
        partkey = std::stoi(tmp);
        tmp = "";
        while(l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }
        i++;
        suppkey = std::stoi(tmp);
        tmp = "";
        while(l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }
        i++;
        availqty = std::stoi(tmp);
        tmp = "";
        while(l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }
        i++;
        supplycost = std::stof(tmp);
        value = supplycost*availqty;
    }

};

bool comp(std::pair<int,long double> p1, std::pair<int,long double> p2) {
    
    return (p1.second > p2.second);
}

int main() {
    std::ifstream nationfile;
    std::ifstream supplierFile;
    std::ifstream partsuppFile;
    std::string line;
    std::unordered_map<std::string,std::string> hNationTable;
    std::unordered_map<std::string,std::string> hSupplyTable;
    std::unordered_map<int,long double> partsupps;
    std::unordered_map<int,long double> partsuppsTable;
    std::vector<PartSupp> psvector;
    auto start = std::chrono::high_resolution_clock::now();
    nationfile.open("/Users/shiro/tpch-dbgen/nation.tbl");
    int c = 0;
    while (std::getline(nationfile,line)) {
        int i = 0;
        std::string n = "";
        std::string key = "";
        while (line[i] != '|') {
            key = key + line[i];
            i++;
        }
        i++;
        while (line[i] != '|') {
            n = n + line[i];
            i++;
        }
        if (n == "GERMANY") {
            hNationTable[key] = n;
        }
    }

    nationfile.close();
    supplierFile.open("/Users/shiro/tpch-dbgen/supplier.tbl");
    while (std::getline(supplierFile,line)) {
        int counter = 0;
        int i = 0;
        std::string s = "";
        std::string key = "";
        while (line[i] != '|') {
            key = key + line[i];
            i++;
        }
        counter++;
        i++;
        while (counter != 3) {
            if (line[i] == '|') {
                counter++;
            }
            i++;
        }
        while (line[i] != '|') {
            s = s + line[i];
            i++;
        }

        if (hNationTable.count(s)) {
            hSupplyTable[key] = s;
        }
    }

    supplierFile.close();
    long double overallvalue = 0;
    partsuppFile.open("/Users/shiro/tpch-dbgen/partsupp.tbl");
    while (std::getline(partsuppFile,line)) {
        c++;
        int i = 0;
        std::string s = "";
        std::string suppkey = "";
        while (line[i] != '|') {
            i++;
        }
        i++;
        while (line[i] != '|') {
            suppkey = suppkey + line[i];
            i++;
        }
        

        if (hSupplyTable.count(suppkey)) {  
            PartSupp ps(line);
            overallvalue += ps.value;
            if (partsupps.count(ps.partkey)) {
                partsupps[ps.partkey] = partsupps[ps.partkey] + ps.value;
            } else {
                partsupps[ps.partkey] = ps.value;
            }
        }
    }


    for (auto ps: partsupps) {
        if (ps.second > 0.0001 * overallvalue) {
            if (partsuppsTable.count(ps.first)) {
                partsuppsTable[ps.first]+=ps.second;
            } else {
                partsuppsTable[ps.first] = ps.second;
            }
        }
    }


    
    partsuppFile.close();

    std::vector<std::pair<int,long double>>orderedpartsupps(partsuppsTable.begin(),partsuppsTable.end());


    std::sort(orderedpartsupps.begin(), orderedpartsupps.end(), comp);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    // for (int i = 0; i < orderedpartsupps.size(); i++) {
    //     std::cout << orderedpartsupps[i].first << "  " << orderedpartsupps[i].second << std::endl;
    // }

    std::cout << "duration = " << duration.count() << std::endl;
    std::cout << "result size = " << partsuppsTable.size() << std::endl;
    


    return 0;
}