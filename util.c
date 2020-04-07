#include <stdlib.h>

int dict[100] = {};
int* insn_ptr;
int i = 0;

void log_insn(int i){
    dict[i] = dict[i] + 1;
}

void init_insn_ptr(int* C){
    insn_ptr = C;
}

void store_int(int a){
    insn_ptr[i] = a;
    i = i + 1;
}

void copy_counts(int* C){
    memcpy(C, dict, 100);
}

int main(int argc, char **argv) {
    int r = run(atoi(argv[1]));
    printf("%d\n", r);
    return 0;
}