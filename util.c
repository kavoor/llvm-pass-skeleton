#include <stdio.h>
#include <string.h> 
#include <limits.h>
#include <stdbool.h>
#include <stdatomic.h>

// Instruction count array
// add = 0
int dict[__INT16_MAX__] = {};

void log_insn(int i){
    dict[i] = dict[i] + 1;
}

// print all the results for each function
void print_results() {
    for(int j=0; j<__INT16_MAX__; j++){
        if (dict[j] != 0){
            printf("%d,",dict[j]);
        }
    } 
    printf("\n");
}
void print_string(int i){
    char c = i;
    printf("%c", c);
}