#include <cstdint>
#include <atomic>

uint64_t loadMemmode(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t size, int n){
    uint64_t res=0;
    for(int i=0; i<size; ++i) {
        uint64_t t1 = __atomic_load_n(&a[i], n);
        uint64_t t2 = __atomic_load_n(&b[i], n);
        uint64_t t3 = __atomic_load_n(&c[i], n);
        res+=t1+t2+t3;
    }
    return res;
}

void storeMemmode(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t size, int n){
    uint64_t val=1;
    for(int i=0; i<size; ++i) {
        __atomic_store_n(&a[i], val, n);
        __atomic_store_n(&b[i], val, n);
        __atomic_store_n(&c[i], val, n);
    }
}