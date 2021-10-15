#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>




int main() {
    std::unordered_map<std::string,std::vector<std::string>> map;
    std::fstream file;
    std::string line;
    double revenue = 0;
    auto start = std::chrono::high_resolution_clock::now();
    file.open("/Users/x/tpch-dbgen/part.tbl");
    while (std::getline(file,line)) {
        std::string key = "";
        std::string size = "";
        std::string brand = "";
        std::string container = "";
        int i = 0;
        while (line[i] != '|') {
            key = key+line[i];
            i++;
        }
        i++;
        int counter = 1;
        while (counter != 3) {
            if (line[i] == '|') {
                counter++;
            }
            i++;
        }
        while (line[i] != '|') {
            brand = brand+line[i];
            i++;
        }
        counter++;
        i++;
        while (counter != 5) {
            if (line[i] == '|') {
                counter++;
            }
            i++;
        }
        while (line[i] != '|') {
            size = size+line[i];
            i++;  
        }
        i++;
        counter++;
        while (line[i] != '|') {
            container = container+line[i];
            i++;
        }

        int sizeI = std::stoi(size);

        if ((brand == "Brand#12" && (container == "SM CASE" || container == "SM BOX" || container == "SM PACK" || container == "SM PKG") &&
            (sizeI >= 1 && sizeI <= 5)) || 
            (brand == "Brand#23" && (container == "MED BAG" || container == "MED BOX" || container == "MED PKG" || container == "MED PACK") &&
            (sizeI >= 1 && sizeI <= 10)) || 
            (brand == "Brand#34" && (container == "LG CASE" || container == "LG BOX" || container == "LG PACK" || container == "LG PKG") &&
            (sizeI >= 1 && sizeI <= 15))) {
                        std::vector<std::string> val;
                        val.push_back(brand);
                        val.push_back(size);
                        val.push_back(container);
                        map[key] = val;
        }
        
    }
    file.close();
    file.open("/Users/x/tpch-dbgen/lineitem.tbl");
    while (std::getline(file,line)) {
        std::string key = "";
        int i = 0;
        while (line[i] != '|') {
            i++;
        }
        i++;
        int counter = 1;
        while (line[i] != '|') {
            key = key+line[i];
            i++;
        }

        i++;
        counter++;

        if (map.count(key)) {
            int quantity = 0;
            double extendedprice = 0;
            double discount = 0;
            std::string shipinstruct = "";
            std::string shipmode = "";
            std::string tmp = "";
            std::string check = map[key][0];
            if (check == "Brand#12") {
                while (counter != 4) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                quantity = std::stoi(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                extendedprice = std::stod(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                i++;
                counter++;
                discount = std::stod(tmp);
                tmp = "";

                while (counter != 13) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    shipinstruct = shipinstruct + line[i];
                    i++;
                }
                i++;
                counter++;
                while (line[i] != '|') {
                    shipmode = shipmode + line[i];
                    i++;
                }
                
                if (quantity >= 1 && quantity <= 11 && shipmode == "AIR" && shipinstruct == "DELIVER IN PERSON") {
                    revenue = revenue + extendedprice * (1-discount);
                }
            } else if (check == "Brand#23") {
                while (counter != 4) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                quantity = std::stoi(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                extendedprice = std::stod(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                i++;
                counter++;
                discount = std::stod(tmp);
                tmp = "";
                
                while (counter != 13) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    shipinstruct = shipinstruct + line[i];
                    i++;
                }
                i++;
                counter++;
                while (line[i] != '|') {
                    shipmode = shipmode + line[i];
                    i++;
                }
                if (quantity >= 10 && quantity <= 20 && shipmode == "AIR" && shipinstruct == "DELIVER IN PERSON") {
                    revenue = revenue + extendedprice * (1-discount);
                }
            } else if (check == "Brand#34") {
                while (counter != 4) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                quantity = std::stoi(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                extendedprice = std::stod(tmp);
                tmp = "";
                i++;
                counter++;
                while (line[i] != '|') {
                    tmp = tmp + line[i];
                    i++;
                }
                counter++;
                i++;
                discount = std::stod(tmp);
                tmp = "";
                
                while (counter != 13) {
                    if (line[i] == '|') {
                        counter++;
                    }
                    i++;
                }
                while (line[i] != '|') {
                    shipinstruct = shipinstruct + line[i];
                    i++;
                }
                i++;
                counter++;
                while (line[i] != '|') {
                    shipmode = shipmode + line[i];
                    i++;
                }
                if (quantity >= 20 && quantity <= 30 && shipmode == "AIR" && shipinstruct == "DELIVER IN PERSON") {
                    revenue = revenue + extendedprice * (1-discount);
                }

            }
        }
    }
    file.close();
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::fixed << revenue << std::endl;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "duration = " << duration.count() << std::endl;
    return 0;
}
