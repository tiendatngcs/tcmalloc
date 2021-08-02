#include <iostream>
#include <cstddef>
#include <unistd.h>

int main() {
    // std::cout << "Standard Alignment: " << alignof(std::max_align_t) << '\n';

    // double *ptr = (double*) malloc(sizeof(double));
    // std::cout << "Double Alignment: " << alignof(*ptr) << '\n';
    // std::cout << "Sleeping..." << std::endl;
    // sleep(5);
    // std::cout << "Pointer: " << ptr << '\n';
    // free(ptr);
    // std::cout << "Pointer: " << ptr << '\n';
    // std::cout << "Char Alignment: " << alignof(*ptr2) << '\n';
    // std::cout << "Sleeping..." << std::endl;
    // std::cout << "Program started ________________________________________\n";
    // int64_t* ptr1 = new int64_t;
    // delete ptr1;
    char* ptr1 = (char*) malloc(64);
    char* ptr2;
    sleep(1);
    for (int i = 0; i < 100; i ++){
        ptr2 = (char*) malloc(1 << 15);
        free(ptr2);
    }
    for (int i = 0; i < 60; i ++){
        ptr2 = (char*) malloc(1 << 16);
        free(ptr2);
    }
    // sleep(10);
    ptr1 = (char*) malloc(32);
    ptr1 = (char*) malloc(128);
    free(ptr2);
    free(ptr1);
    // void *ptr3;
    // std::cout << "Sizeof void*: " << sizeof(ptr3) << '\n';
    return 0;
}