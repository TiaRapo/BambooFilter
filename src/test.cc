// Use this file for testing until the project is finished
// TODO: Delete this file

#include <iostream>
#include <cstddef>
#include <span>

#include "bamboo_filter.h"

int main(int argc, char* argv[]) {
    std::cout << "Your active compiler C++ standard version: " << __cplusplus << '\n';
    std::cout << "Should be at least 202002\n";
    
    BambooFilter* bf = new BambooFilter(1024);

    std::cout << "t1:\n" << *bf << '\n';

    bf->Insert(std::as_bytes(std::span("test1", 5)));

    std::cout << "t2:\n" << *bf << '\n';

    return 0;
}