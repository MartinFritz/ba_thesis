# ba_thesis

benchmarksuite.cpp contains the benchmarks that were used during our thesis. We recommend checking the performance of the TPC-H queries without google benchmark.
Everything was compiled with gcc-11/g++-11.
The header files need to be in the same folder as the benchmarksuite.cpp. The folder hierarchy presented in this repo is mainly for clarity.

Native compilation:
g++-11 -std=c++17 -I"/Users/x/benchmark/build/src" -I"/Users/x/benchmark/include" -L"/Users/x/benchmark/include" -L"/Users/x/benchmark/build/src" -I"/opt/homebrew/include" -L"/opt/homebrew/lib" -pthread -lbenchmark -fopenmp -ltbb benchmarksuite.cpp -O3

Rosetta compilation:
x86_64-apple-darwin20-g++-11 -std=c++17 -I"/Users/x/benchmark-rosetta/build/src" -I"/Users/x/benchmark-rosetta/include" -L"/Users/x/benchmark-rosetta/include" -L"/Users/x/benchmark-rosetta/build/src" -I"/usr/local/include" -L"/usr/local/lib" -pthread -lbenchmark -ltbb -fopenmp benchmarksuite.cpp -O3


Please do adapt the commands with the right paths to the google benchmark, pthread, Intel oneTBB folders. 
