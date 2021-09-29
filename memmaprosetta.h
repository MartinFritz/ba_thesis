#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread/pthread.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread/pthread.h>
#include <libkern/OSCacheControl.h>


uint8_t codeARM[] = {   
       0xE3, 0x03, 0x00, 0xAA, //mov x3,x0
       0x01, 0x00, 0x80, 0xD2, //mov x1,0
       0x3F, 0x00, 0x03, 0xEB, //cmp x1,x3
       0x80, 0x00, 0x00, 0x54, //beq 36
       0x21, 0x04, 0x00, 0x91, //add x1, x1, 1
       0xE0, 0x03, 0x01, 0xAA, //mov x0,x1
       0xFC, 0xFF, 0xFF, 0x17, //b 16
       0xC0, 0x03, 0x5F, 0xD6};//ret

uint8_t codeIntel[] = {
     0x48, 0x89, 0xc1,                         //mov rcx, rax
     0x48, 0xC7, 0xc3, 0x00, 0x00, 0x00, 0x00, //mov rbx, 0
     0x48, 0x39, 0xd9,                         //cmp rcx, rbx
     0x74, 0x08,                               //je +8
     0x48, 0xff, 0xc3,                         //inc rbx
     0x48, 0x89, 0xd8,                         //mov rax,rbx
     0xeb, 0xf4,                               //jmp -13
   //  0x48, 0xc7, 0xc0, 0x00, 0x01, 0x00, 0x00, //mov rax, 0x100
     0xc3                                   //ret
    };

long (*codeArr_arm)(long) = reinterpret_cast<long (*) (long)>(mmap(NULL, sizeof(codeARM), PROT_READ|PROT_EXEC|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_JIT, -1, 0));
long (*codeArr_intel)(long) = reinterpret_cast<long (*) (long)>(mmap(NULL, sizeof(codeIntel), PROT_READ|PROT_EXEC|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_JIT, -1, 0));


void setup_arm() {
    pthread_jit_write_protect_np(0);
    
    memcpy(reinterpret_cast<void*> (codeArr_arm),codeARM,sizeof(codeARM));

    pthread_jit_write_protect_np(1);

    sys_icache_invalidate(reinterpret_cast<void*> (codeArr_arm), sizeof(codeARM));
    return;
}

long memmaprosetta_arm(long n) {
    
    long test = codeArr_arm(n);
    return test;
}

void setup_intel() {
    pthread_jit_write_protect_np(0);
    
    memcpy(reinterpret_cast<void*> (codeArr_intel),codeIntel,sizeof(codeIntel));

    pthread_jit_write_protect_np(1);

    sys_icache_invalidate(reinterpret_cast<void*> (codeArr_intel), sizeof(codeIntel));
    return;
}

long memmaprosetta_intel(long n) {
    
    long test = codeArr_intel(n);
    return test;
}

  