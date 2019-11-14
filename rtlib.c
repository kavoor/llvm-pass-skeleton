#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

// int srcmap[INT_MAX];
// int destmap[INT_MAX];
int WTF[__INT16_MAX__][__INT16_MAX__] = {};

bool init = 0;
bool previousSource = false;
int previousSourceNum;

void logsrc(int i) {
    previousSource = true;
    previousSourceNum = i;
}


void logdest(int i) {
    if(!previousSource) return;
    // if( !init ){
    //     memset(srcmap, 0, INT_MAX);
    //     memset(destmap, 0, INT_MAX);
    //     init = 1;
    // }
    // destmap[i]++;
    // srcmap[previousSourceNum]++;
    WTF[previousSourceNum][i]++;
    // printf(">>SOURCE %d, DEST %d, COUNT %d \n", previousSourceNum, i, WTF[previousSourceNum][i]);
    // printf(">>SOURCE %d, %d \n", previousSourceNum, srcmap[previousSourceNum]);
    // printf(">>DEST %d, %d \n", i, destmap[i]);
    previousSource = false;
}

// print all the results for each function
void print_results() {
    printf("  Edge        Count\n");
    for(int i=0; i<__INT16_MAX__; i++){
       for(int j=0; j<__INT16_MAX__; j++){
           if (WTF[i][j] != 0){
               printf("[%d -> %d]        %d\n",i,j,WTF[i][j]);
           }
        } 
    }
}