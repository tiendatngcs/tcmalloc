#include "tcmalloc/malloc_extension.h"
#include <iostream>
#include <string>

int main(){
    std::cout << tcmalloc::MallocExtension::GetStats() << std::endl;
    // std::cout << "Success" << std::endl;
}