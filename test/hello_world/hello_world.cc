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

    char *ptr2 = (char*) malloc(1);
    // std::cout << "Char Alignment: " << alignof(*ptr2) << '\n';
    // std::cout << "Sleeping..." << std::endl;
    free(ptr2);
    // void *ptr3;
    // std::cout << "Sizeof void*: " << sizeof(ptr3) << '\n';
    sleep(10);
    return 0;
}