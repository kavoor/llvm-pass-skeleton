#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

int srcmap[INT_MAX];
int destmap[INT_MAX];
bool init = 0;

void logsrc(int i) {
    if( !init ){
        memset(srcmap, 0, INT_MAX);
        init = 1;
    }
    srcmap[i]++;
    printf(">>SOURCE %d, %d \n", i, srcmap[i]);
}


void logdest(int i) {
    if( !init ){
        memset(destmap, 0, INT_MAX);
        init = 1;
    }
    destmap[i]++;
    printf(">>DEST %d, %d \n", i, destmap[i]);
}