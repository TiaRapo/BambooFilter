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
    std::span<const std::byte> element = std::as_bytes(std::span("test1", 5));

    std::cout << "BF at t0:\n" << *bf << '\n';
    std::cout << "\tTesting insert (expected 1): " << bf->Insert(element) << '\n';

    std::cout << "BF at t1:\n" << *bf << '\n';
    std::cout << "\tTesting lookup (expected 0): " << bf->Lookup(element) << "\n";
    std::cout << "\tTesting delete (expected 0): " << bf->Delete(element) << "\n";

    std::cout << "BF at t2:\n" << *bf << '\n';
    std::cout << "\tTesting insert (expected 1): " << bf->Lookup(element) << "\n";
    std::cout << "\tTesting lookup (expected 1): " << bf->Lookup(element) << "\n";

    std::cout << "BF at t3:\n" << *bf << '\n';
    std::cout << "\tTesting delete (expected 1): " << bf->Delete(element) << "\n";
    std::cout << "\tTesting lookup (expected 0): " << bf->Lookup(element) << "\n";

    std::cout << "BF at t4:\n" << *bf << '\n';

    return 0;
}