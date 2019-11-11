#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>

int map[INT_MAX];
bool init = 0;

void logop(int i) {
    if( !init ){
        memset(map, 0, INT_MAX);
        init = 1;
    }
}

void inc(int key){
    map[key] = map[key] + 1;
    printf("%d, %d", key, map[key]);
}

