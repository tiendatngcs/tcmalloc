#include <iostream>
#include <cstddef>
#include <unistd.h>
#include "malloc_extension.h"

int main() {
    std::cout << "Standard Alignment: " << alignof(std::max_align_t) << '\n';

    double *ptr = (double*) malloc(sizeof(double));
    std::cout << "Double Alignment: " << alignof(*ptr) << '\n';
    std::cout << "Sleeping..." << std::endl;
    sleep(5);

    char *ptr2 = (char*) malloc(1);
    std::cout << "Char Alignment: " << alignof(*ptr2) << '\n';
    std::cout << "Sleeping..." << std::endl;
    sleep(5);

    void *ptr3;
    std::cout << "Sizeof void*: " << sizeof(ptr3) << '\n';
    tcmalloc::MallocExtension::GetStats()
    return 0;
}