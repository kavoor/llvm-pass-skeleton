#include <stdio.h>
int main(int argc, const char** argv) {
    int num;
    scanf("%i", &num);

    for(int i =0; i<5; i++){
        printf("Iteration %i\n", i);
    }

    if(num%2 == 0){
        printf("branch1\n");
    }
    else{
        printf("branch2\n");
    }

    printf("%i\n", num + 2);
    return 0;
}
