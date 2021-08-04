#include <iostream>
#include <cstddef>
#include <unistd.h>
#include <stdlib.h>

int main() {
    srand (time(NULL));
    int num;
    char* ptr1;
    for (int i = 0; i < 600; i++){
        num = rand() % (1<<17) + 1;
        // printf("%d\n", num);
        ptr1 = (char*) malloc(num);
        // sleep(0.5);
        free(ptr1);

    }

    // char* ptr2;
    // sleep(1);
    // for (int i = 0; i < 100; i ++){
    //     ptr2 = (char*) malloc(1 << 15);
    //     free(ptr2);
    // }
    // for (int i = 0; i < 60; i ++){
    //     ptr2 = (char*) malloc(1 << 16);
    //     free(ptr2);
    // }
    // // sleep(10);
    // ptr1 = (char*) malloc(32);
    // ptr1 = (char*) malloc(128);
    // free(ptr2);
    // free(ptr1);
    // void *ptr3;
    // std::cout << "Sizeof void*: " << sizeof(ptr3) << '\n';

    // if (0){
    //     printf("aa");
    // }
    return 0;
}