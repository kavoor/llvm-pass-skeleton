#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

std::atomic<int> map[INT_MAX];
bool init = 0;


void inc(int key){
    map[key] = map[key] + 1;
    printf("%d, %d \n", key, map[key]);
}


void logop(int i) {
    if( !init ){
        memset(map, 0, INT_MAX);
        init = 1;
    }
    inc(i);
}
