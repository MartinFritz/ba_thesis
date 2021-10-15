#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
class Part {
public:
    std::string p_brand, p_container;
    int p_size;
    int p_partkey;
    std::string tmp = "";

    Part(std::string l) {
        int i = 0;
        int count = 0;
        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        p_partkey = std::stoi(tmp);

        tmp = "";

        while (i < l.size() && count < 4) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        p_brand = tmp;
        tmp = "";

        while (i < l.size() && count < 7) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }

        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        p_size = std::stoi(tmp);
        tmp = "";
        i++;

        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        p_container = tmp;
    }
    ~Part() {

    }

};

class LineItem {
public:
    std::string l_shipmode, l_shipinstruct;
    int l_quantity, l_partkey;
    double l_extendedprice;
    std::string tmp = "";
    double l_discount;
    LineItem(std::string l) {
        int i = 0;
        int count = 0;
        while (i < l.size() && count < 1) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        
        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        l_partkey = std::stoi(tmp);
        tmp = "";

        while (i < l.size() && count < 5) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }
        
        count++;
        l_quantity = std::stoi(tmp);
        tmp = "";
        i++;

        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        l_extendedprice = std::stod(tmp);
        tmp = "";
        i++;

        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        count++;
        l_discount = std::stod(tmp);
        tmp = "";


        while (i < l.size() && count < 15) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }


        count++;
        l_shipinstruct = tmp;
        tmp = "";
        i++;

        while (i < l.size() && l[i] != '|') {
            tmp = tmp + l[i];
            i++;
        }

        l_shipmode = tmp;
        
    }
    ~LineItem() {

    }

};



int main() {
    std::ifstream lineitemFile;
    std::ifstream partFile;
    std::string line;
    std::string line2;
    std::vector<Part> partsBrand12;
    std::vector<Part> partsBrand23;
    std::vector<Part> partsBrand34;
    std::vector<LineItem> lineitems;
    double revenue = 0;
    auto start = std::chrono::high_resolution_clock::now();
    partFile.open("/Users/shiro/tpch-dbgen/part.tbl");


    while(std::getline(partFile,line)) {

        Part Part(line);

        if (Part.p_brand == "Brand#12") {
            if (Part.p_container == "SM CASE" || Part.p_container == "SM BOX" || Part.p_container == "SM PKG" ||
            Part.p_container == "SM PACK") {

                if (Part.p_size >= 1 && Part.p_size <= 5) {
                    
                    partsBrand12.push_back(Part);
                }
            }
        }
        if (Part.p_brand == "Brand#23") {
            if (Part.p_container == "MED BAG" || Part.p_container == "MED BOX" || Part.p_container == "MED PKG" ||
            Part.p_container == "MED PACK") {
                if (Part.p_size >= 1 && Part.p_size <= 10) {
                    partsBrand23.push_back(Part);
                }
            }
        }
        if (Part.p_brand == "Brand#34") {
            if (Part.p_container == "LG CASE" || Part.p_container == "LG BOX" || Part.p_container == "LG PKG" ||
            Part.p_container == "LG PACK") {
                if (Part.p_size >= 1 && Part.p_size <= 15) {
                    partsBrand34.push_back(Part);
                }
            }
        }
    }
    partFile.close();
    
    lineitemFile.open("/Users/shiro/tpch-dbgen/lineitem.tbl");
    while(std::getline(lineitemFile,line)) {
        LineItem lineItem(line);
        if (lineItem.l_shipinstruct == "DELIVER IN PERSON" && (lineItem.l_shipmode == "AIR" 
                                                                || lineItem.l_shipmode == "AIR REG")) {
            if (lineItem.l_quantity >= 1 && lineItem.l_quantity <= 11) {
                for (int i = 0; i < partsBrand12.size(); i++) {
                    if (lineItem.l_partkey == partsBrand12[i].p_partkey) {
                        revenue = revenue + lineItem.l_extendedprice * (1 - lineItem.l_discount);
                        break;
                    }
                }
            }
            if (lineItem.l_quantity >= 10 && lineItem.l_quantity <= 20) {
                for (int i = 0; i < partsBrand23.size(); i++) {
                    if (lineItem.l_partkey == partsBrand23[i].p_partkey) {
                        revenue = revenue + lineItem.l_extendedprice * (1 - lineItem.l_discount);
                        break;
                    }
                }
            }
            if (lineItem.l_quantity >= 20 && lineItem.l_quantity <= 30) {
                for (int i = 0; i < partsBrand34.size(); i++) {
                    if (lineItem.l_partkey == partsBrand34[i].p_partkey) {
                        revenue = revenue + lineItem.l_extendedprice * (1 - lineItem.l_discount);
                        break;
                    }
                }
            }
        }
    }
    lineitemFile.close();
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << std::fixed << "revenue = " << revenue << std::endl;
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "duration = " << duration.count() << std::endl;
    return 0;
}