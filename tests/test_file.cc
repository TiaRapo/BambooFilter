#include <iostream>
#include <cstddef>
#include <span>
#include <vector>
#include <set>
#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>

#include <filesystem>

#include "bamboo_filter.h"
#include "timing.h"

int main(int argc, char* argv[]) {
    // GCA_030708175.1_ASM3070817v1_genomic.fna

    uint32_t num_operations = static_cast<uint32_t>(std::stoul(argv[1]));
    int k = atoi(argv[2]);
    std::string filename = argv[3];

    std::ifstream in_file(filename);
    std::ofstream out_file("/tests/output/test_file.txt");

    std::string line;
    std::string kmer = "";
    std::vector<std::string> insert_strings, lookup_strings;
    std::vector<std::span<const std::byte>> insert_bytes, lookup_bytes;
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, 3);
    char bases[] = "ATCG";


    // std::cout << std::filesystem::current_path() << "\n";

    std::cout << "K: " << k << "\n";
    std::cout << "Num of operations: " << num_operations << "\n";
    std::cout << "\n";


    std::cout << "Preparing data\n";

    // Read first line that is file descriptor
    getline(in_file, line);

    // Load all kMers
    // int counter = 0;
    while(getline(in_file, line)) {
        kmer += line;
        // std::cout << line << "\n";
        while (kmer.length() >= k) {
            // std::cout << kmer.substr(0, k) << "\n";
            insert_strings.push_back(kmer.substr(0, k));
            kmer.erase(0, 1);
            // counter++;
        }
        // if (counter>=num_operations) break;
    }
    for (std::string& s : insert_strings) {
        insert_bytes.push_back(std::as_bytes(std::span(s)));
    }

    // Free space
    insert_strings.clear();
    insert_strings.shrink_to_fit();

    std::cout << "\tInsert_strings size: " << insert_strings.size() << "\n";
    std::cout << "\tInsert_bytes size: " << insert_bytes.size() << "\n";

    // Generating random kmers for lookup
    for (int i = 0; i < num_operations; i++) {
        kmer = "";
        for (int j = 0; j < k; j++) {
            kmer += bases[distribution(generator)];
        }
        lookup_strings.push_back(kmer);
    }
    for (std::string& s : lookup_strings) {
        lookup_bytes.push_back(std::as_bytes(std::span(s)));
    }

    // Free space
    lookup_strings.clear();
    lookup_strings.shrink_to_fit();

    std::cout << "\tLookup_strings size: " << lookup_strings.size() << "\n";
    std::cout << "\tLookup_bytes size: " << lookup_bytes.size() << "\n";


    std::cout << "Initial capacity: " << (1u << (int)ceil(log2(insert_bytes.size()))) << "\n";

    std::cout << "\n";



    BambooFilter* bf = new BambooFilter((1u << (int)ceil(log2(insert_bytes.size()))));

    // Start timer--Insert
    std::cout << "Inserting genome\n";
    auto start_time = NowNanos();

    int counter = 0;
    for (std::span<const std::byte> element : insert_bytes) {
        bf->Insert(element);
        if (counter % 10000 == 0) std::cout << "CAP: "<< bf->GetCapacity() << "\tELEM: " << bf->GetNumElems() << "\n";
        counter++;
        if (counter == 1000000) break;
    }

    std::cout << "Insert rate: " << ((insert_bytes.size() * 1000.0) / static_cast<double>(NowNanos() - start_time)) << "\n";

    // Start timer--Lookup
    std::cout << "Looking for random kMers\n";
    start_time = NowNanos();

    for (std::span<const std::byte> element : lookup_bytes) {
        bf->Lookup(element);
    }

    std::cout << "Insert rate: " << ((lookup_bytes.size() * 1000.0) / static_cast<double>(NowNanos() - start_time)) << "\n";

    delete bf;

    return 0;
}