#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

// int srcmap[INT_MAX];
// int destmap[INT_MAX];
int WTF[__INT16_MAX__][__INT16_MAX__] = {};

bool previousSource = false;
int previousSourceNum;

void logsrc(int i) {
    previousSource = true;
    previousSourceNum = i;
}


void logdest(int i) {
    if(!previousSource) return;
    WTF[previousSourceNum][i]++;
    previousSource = false;
}

void logdisrupt(int i){
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