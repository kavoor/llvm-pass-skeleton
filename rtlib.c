#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

int srcmap[INT_MAX];
int destmap[INT_MAX];
bool init = 0;
bool previousSource = false;
int previousSourceNum;

void logsrc(int i) {
    previousSource = true;
    previousSourceNum = i;
}


void logdest(int i) {
    if(!previousSource) return;
    if( !init ){
        memset(srcmap, 0, INT_MAX);
        memset(destmap, 0, INT_MAX);
        init = 1;
    }
    destmap[i]++;
    srcmap[previousSourceNum]++;
    printf(">>SOURCE %d, %d \n", previousSourceNum, srcmap[previousSourceNum]);
    printf(">>DEST %d, %d \n", i, destmap[i]);
    previousSource = false;
}