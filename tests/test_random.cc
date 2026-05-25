#include <iostream>
#include <cstddef>
#include <span>
#include <random>
#include <algorithm>
#include <vector>
#include <string>

#include "random.h"
#include "timing.h"

#include "bamboo_filter.h"

int main(int argc, char* argv[]) {
    std::cout << "### test_random ###\n";
    
    // TODO: VERY UNFINISHED

    std::vector<std::string> to_add;
    std::vector<std::string> to_lookup;
    
    GenerateRandom64(100000, to_add, to_lookup);
    
    std::vector<std::span<const std::byte>> to_add_bytes;
    std::vector<std::span<const std::byte>> to_lookup_bytes;

    for (std::string& elem : to_add) {
        to_add_bytes.push_back(std::as_bytes(std::span(elem)));
    }

    for (std::string& elem : to_lookup) {
        to_lookup_bytes.push_back(std::as_bytes(std::span(elem)));
    }

    BambooFilter* bf = new BambooFilter(20000);

    std::size_t insert_count = 0;
    for (auto& elem : to_add_bytes) {
        if (bf->Insert(elem)) {
            insert_count++;
        }
    }

    std::size_t found_count = 0;
    for (auto& elem : to_lookup_bytes) {
        if (bf->Lookup(elem)) {
            found_count++;
        }
    }

    std::cout << "Insert count: " << insert_count << '\n';
    std::cout << "Found count: " << found_count << ", tried " << to_lookup_bytes.size() << '\n';

    return 0;
}