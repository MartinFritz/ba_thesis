#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;
class Data {
    public:
    std::string shipDate;
    double discount;
    double quantity;
    double extendedPrice;
    bool query6() {
        return (    shipDate >= "1994-01-01"
                &&  shipDate < "1995-01-01"
                &&  discount >= (0.05)
                &&  discount <= (0.07)
                &&  quantity < 24
                );
    }

    Data(string l) {
        int count = 0;
        int i = 0;
        string shipD = "";
        string quant = "";
        string disc = "";
        string extPr = "";
        while (i < l.size() && count < 4) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        while (i < l.size() && l[i] != '|') {
            quant = quant + l[i];
            i++;
        }
        i++;
        count++;
        quantity = stod(quant);
        
        while (i < l.size() && l[i] != '|') {
            extPr = extPr + l[i];
            i++;
        }
        count++;
        i++;
        extendedPrice = stod(extPr);
        
        while (i < l.size() && l[i] != '|') {
            disc = disc + l[i];
            i++;
        }
        count++;
        i++;
        discount = stod(disc);
        while (i < l.size() && count < 10) {
            if (l[i] == '|') {
                count++;
            }
            i++;
        }
        while (i < l.size() && l[i] != '|') {
            shipD = shipD + l[i];
            i++;
        }
        shipDate = shipD;
        
    }


    
};


int main() {
    long double queryRes = 0;
    ifstream file;
    string line;
    file.open("/Users/x/tpch-dbgen/lineitem.tbl");
    auto start = std::chrono::high_resolution_clock::now();
    while (getline(file,line)) {
        Data data(line);
        
        if (data.query6()) {
            queryRes = queryRes + (data.extendedPrice*data.discount);

        }

    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    file.close();
    std::cout << std::fixed << queryRes << std::endl;
    std::cout << duration.count() << std::endl;

    
}
