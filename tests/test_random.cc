#include <iostream>
#include <cstddef>
#include <span>
#include <random>
#include <algorithm>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <malloc.h>

#include "random.h"
#include "timing.h"

#include "bamboo_filter.h"

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

size_t get_heap_usage() {
    struct mallinfo2 mi = mallinfo2();
    return mi.uordblks; // total space allocated to program in bytes;
}

int main(int argc, char* argv[]) {
    uint32_t num_operations = static_cast<uint32_t>(std::stoul(argv[1]));

    std::ofstream out_file("./output/test_random.txt", std::ios::app);

    std::vector<std::string> to_add;
    std::vector<std::string> to_lookup;
    
    std::cout << "Preparing random data...\n";
    GenerateRandom64SeededMT(num_operations, to_add, to_lookup, 42);
    
    std::vector<std::span<const std::byte>> to_add_bytes;
    std::vector<std::span<const std::byte>> to_lookup_bytes;

    for (std::string& elem : to_add) {
        to_add_bytes.push_back(std::as_bytes(std::span(elem)));
    }

    for (std::string& elem : to_lookup) {
        to_lookup_bytes.push_back(std::as_bytes(std::span(elem)));
    }

    size_t mem1 = get_heap_usage();
    std::cout << "MEM: " << mem1 << "B" << std::endl;

    BambooFilter* bf = new BambooFilter(num_operations);

    std::cout << "Inserting random elements\n";
    auto start_time = NowNanos();

    std::size_t insert_count = 0;
    for (auto& elem : to_add_bytes) {
        if (bf->Insert(elem)) {
            insert_count++;
        }
    }

    auto insert_rate = ((insert_count * 1000.0) / static_cast<double>(NowNanos() - start_time));
    std::cout << "Insert rate: " << insert_rate << "\n";

    // Start timer--Lookup
    std::cout << "Looking for random elements\n";
    start_time = NowNanos();

    std::size_t found_count = 0;
    for (auto& elem : to_lookup_bytes) {
        if (bf->Lookup(elem)) {
            found_count++;
        }
    }

    auto lookup_rate = ((to_lookup_bytes.size() * 1000.0) / static_cast<double>(NowNanos() - start_time));
    std::cout << "Lookup rate: " << lookup_rate << '\n';
    std::cout << "Insert count: " << insert_count << '\n';
    std::cout << "Found count: " << found_count << ", tried " << to_lookup_bytes.size() << '\n';

    size_t mem2 = get_heap_usage();
    std::cout << "MEM: " << mem2 << "B" << std::endl;
    
    size_t mem_diff = mem2 - mem1;
    std::cout << "MEM_diff: " << mem_diff << std::endl;
    
    out_file << "# insert_lookup_count;insert_rate;lookup_rate;memory(bytes)\n";
    out_file << insert_count << ';';
    out_file << insert_rate << ';';
    out_file << lookup_rate << ';';
    out_file << mem_diff << '\n';

    delete bf;

    return 0;
}