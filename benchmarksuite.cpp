#include <benchmark/benchmark.h>
#include "quicksort.h"
#include "sorttest.h"
#include "query19OWN.h"
#include "query19OWN2.h"
#include "query11OWN.h"
#include "query19OWN2_3.h"
#include "memmaprosetta.h"
#include "ht.h"
#include <omp.h>

static void BM_ht_insert(benchmark::State& state) {
  myHashTable<uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {

      uint32_t k = uni(gen);
      h.insert(k,uni(gen));

    }
  }
}

static void BM_unorderedmap_insert(benchmark::State& state) {
  std::unordered_map<uint32_t,uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());
  
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); i++) {
      uint32_t k = uni(gen);
      h[k] = uni(gen);
    }
  }
}

static void BM_ht_lookup(benchmark::State& state) {
  myHashTable<uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());
  std::vector<uint32_t> keys;
  for (int i = 0; i < state.range(0); i++) {
      uint32_t k = uni(gen);
      h[k] = uni(gen);
      keys.push_back(k);
  }
  random_shuffle(keys.begin(),keys.end());
  for (auto _ : state) {
      for (int i = 0; i < state.range(0); i++) {
        h[keys[i]];
      }
  }
}

static void BM_unorderedmap_lookup(benchmark::State& state) {
  std::unordered_map<uint32_t,uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());
  std::vector<uint32_t> keys;
  for (int i = 0; i < state.range(0); i++) {
      uint32_t k = uni(gen);
      h[k] = uni(gen);
      keys.push_back(k);
  }
  random_shuffle(keys.begin(),keys.end());
  for (auto _ : state) {
      for (int i = 0; i < state.range(0); i++) {
        h[keys[i]];
      }
  }
}


static void BM_ht_delete(benchmark::State& state) {
  myHashTable<uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());
  std::vector<uint32_t> keys;
  for (int i = 0; i < state.range(0); i++) {
      uint32_t k = uni(gen);
      h[k] = uni(gen);
      keys.push_back(k);
  }
  random_shuffle(keys.begin(),keys.end());
  for (auto _ : state) {
      for (int i = 0; i < state.range(0); i++) {
        h.erase(keys[i]);
      }
  }

}

static void BM_unorderedmap_delete(benchmark::State& state) {
  std::unordered_map<uint32_t,uint32_t> h;
  std::mt19937 gen(100);
  std::uniform_int_distribution<uint32_t> uni(0, std::numeric_limits<uint32_t>::max());
  std::vector<uint32_t> keys;
  for (int i = 0; i < state.range(0); i++) {
      uint32_t k = uni(gen);
      h[k] = uni(gen);
      keys.push_back(k);
  }
  random_shuffle(keys.begin(),keys.end());
  for (auto _ : state) {
      for (int i = 0; i < state.range(0); i++) {
        h.erase(keys[i]);
      }
  }

}




static void BM_introsort(benchmark::State& state) {
  std::vector<int> input;
  
  for (auto _ : state) {
    state.PauseTiming();
    input.resize(state.range(0));
    for (int i = 0; i < (state.range(0)); i++) {
      input[i] = (rand() % 1000000);
    }
    state.ResumeTiming();
    std::sort(std::execution::seq, input.begin(), input.end());
  }
  return;
}

static void BM_memmaprosetta_arm(benchmark::State& state) {
  setup_arm();
  for (auto _: state) {
    memmaprosetta_arm(state.range(0));
  }
  return;
}

static void BM_memmaprosetta_intel(benchmark::State& state) {
  setup_intel();
  long x;
  for (auto _: state) {
    x = memmaprosetta_intel(state.range(0));
  }
  return;
}

static void BM_quicksort_startPiv(benchmark::State& state) {
  std::vector<int> input;
  
  for (auto _ : state) {
    state.PauseTiming();
    input.resize(state.range(0));

    for (int i = 0; i < (state.range(0)); i++) {
      input[i] = (rand() % 1000000);
    }

    omp_set_dynamic(0);              /** Explicitly disable dynamic teams **/
    omp_set_num_threads(state.range(1));
    //std::sort(std::execution::par_unseq, input.begin(), input.end());

    state.ResumeTiming();
    #pragma omp parallel 
    {
      #pragma omp single
        quicksort_start(input, 0, input.size()-1);
    }
  }
  return;
}

static void BM_quicksort_endPiv(benchmark::State& state) {
  std::vector<int> input;
  
  for (auto _ : state) {
    state.PauseTiming();
    input.resize(state.range(0));

    for (int i = 0; i < (state.range(0)); i++) {
      input[i] = (rand() % 1000000);
    }
    state.ResumeTiming();
    quick_sort_endPivot(input, 0, input.size()-1);
  }
}

static void BM_quicksort_rdPiv(benchmark::State& state) {
  std::vector<int> input;
  
  for (auto _ : state) {
    state.PauseTiming();
    input.resize(state.range(0));

    for (int i = 0; i < (state.range(0)); i++) {
      input[i] = (rand() % 1000000);
    }
    state.ResumeTiming();
    quick_sort_randomPivot(input, 0, input.size()-1);
  }
}


static void BM_sorttest_generateTuples(benchmark::State& state) {
  vector<Tuple> tuples;
  for (auto _ : state) {
    generateTuples(tuples, state.range(0));
  }
}

static void BM_sorttest_radixsort(benchmark::State& state) {
  vector<Tuple> tuples;
  generateTuples(tuples,state.range(0));
  for (auto _ : state) {
    radixSort(tuples.data(),tuples.data()+tuples.size());
  }
}

static void BM_sorttest__RadixSort_Unsigned_PowerOf2Radix_1(benchmark::State& state) {
  vector<Tuple> tuples;
  generateTuples(tuples, state.range(0));
  for (auto _ : state) {
    _RadixSort_Unsigned_PowerOf2Radix_1<Tuple,256,8,100>(tuples.data(),tuples.size(),static_cast<uint64_t>(0xFF)<<(64-8),64-8);
  }
}

static void BM_sorttest_quicksort(benchmark::State& state) {
  vector<Tuple> tuples;
  generateTuples(tuples, state.range(0));
  for (auto _ : state) {
    quickSort(tuples.data(),tuples.data()+tuples.size());
  }
}

static void BM_query19(benchmark::State& state) {
  for (auto _ : state) {
    query19();
  }
}

static void BM_query19_2(benchmark::State& state) {
  for (auto _ : state) {
    query19_2();
  }
}

static void BM_query11(benchmark::State& state) {
  for (auto _ : state) {
    query11();
  }
}

static void BM_query19_2_3(benchmark::State& state) {
  setupQuery19_2_3();
  double x;
  for (auto _ : state) {
    query19OWN2_3();
  }
  std::cout << x << std::endl;
}



// BENCHMARK(BM_sorttest_generateTuples)->Name("generateTuples")->Arg(1000000)->Arg(500000000)->Arg(1000000000)->Unit(benchmark::kSecond);
// BENCHMARK(BM_sorttest_generateTuples)->Name("generateTuples2")->Arg(1000000)->Arg(500000000)->Arg(1000000000)->Unit(benchmark::kSecond);
// BENCHMARK(BM_sorttest_radixsort)->Name("radixsort")->Arg(1000000)->Arg(500000000)->Arg(1000000000)->Unit(benchmark::kSecond);
// BENCHMARK(BM_quicksort_startPiv)->Name("quicksort_startPiv")->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(1000000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_quicksort_endPiv)->Name("quicksort_endPiv")->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(1000000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_quicksort_rdPiv)->Name("quicksort_randomPiv")->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(1000000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_introsort)->Name("std::introsort")->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(1000000000)->Unit(benchmark::kMillisecond);
// BENCHMARK(BM_sorttest__RadixSort_Unsigned_PowerOf2Radix_1)->Name("radixsUnsignPowOf2Radix1")
//           ->Arg(1000000)->Arg(500000000)->Arg(1000000000)->Unit(benchmark::kSecond);
//BENCHMARK(BM_query19)->Unit(benchmark::kSecond);
//BENCHMARK(BM_query19_2)->Unit(benchmark::kSecond);
//BENCHMARK(BM_query11)->Unit(benchmark::kSecond);
//BENCHMARK(BM_query19_2_3) -> Unit(benchmark::kSecond);
//BENCHMARK(BM_memmaprosetta_arm) ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(2000000000) -> Unit(benchmark::kMillisecond);
//BENCHMARK(BM_memmaprosetta_intel) ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(100000000)->Arg(2000000000) -> Unit(benchmark::kMillisecond);
//BENCHMARK(BM_quicksort_startPiv) -> Name("quicksort_startPiv_1")->Args({10000,1})->Args({100000,1})->Args({1000000,1})->Args({100000000,1})->Args({1000000000,1})->Unit(benchmark::kMillisecond);
//BENCHMARK(BM_quicksort_startPiv) -> Name("quicksort_startPiv_2")->Args({10000,2})->Args({100000,2})->Args({1000000,2})->Args({100000000,2})->Args({1000000000,2})->Unit(benchmark::kMillisecond);
//BENCHMARK(BM_quicksort_startPiv) -> Name("quicksort_startPiv_4")->Args({10000,4})->Args({100000,4})->Args({1000000,4})->Args({100000000,4})->Args({1000000000,4})->Unit(benchmark::kMillisecond);
//BENCHMARK(BM_quicksort_startPiv) -> Name("quicksort_startPiv_8")->Args({10000,8})->Args({100000,8})->Args({1000000,8})->Args({100000000,8})->Args({1000000000,8})->Unit(benchmark::kMillisecond);
//BENCHMARK(BM_ht_insert) -> Name("ht_insert") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);
//BENCHMARK(BM_unorderedmap_insert) -> Name("unorderedmap_insert") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);
//BENCHMARK(BM_ht_lookup) -> Name("ht_lookup") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);
//BENCHMARK(BM_unorderedmap_lookup) -> Name("unorderedmap_lookup") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);

BENCHMARK(BM_unorderedmap_delete) -> Name("unorderedmap_delete") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);

BENCHMARK(BM_ht_delete) -> Name("ht_delete") ->Arg(10000)->Arg(100000)->Arg(1000000)->Arg(10000000)->Arg(100000000) -> Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();