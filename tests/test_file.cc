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
    std::string filepath = argv[1];
    int k = atoi(argv[2]);
    uint32_t num_operations = static_cast<uint32_t>(std::stoul(argv[3]));

    if (!std::filesystem::exists(filepath)) return 1;

    std::ifstream in_file(filepath);
    std::ofstream out_file("./output/test_file.txt", std::ios::app);

    std::string line;
    std::string kmer = "";
    std::vector<std::string> to_add, to_lookup;
    std::vector<std::span<const std::byte>> to_add_bytes, to_lookup_bytes;
    
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, 3);
    char bases[] = "ATCG";


    std::cout << "K: " << k << "\n";
    std::cout << "Num of operations: " << num_operations << "\n";
    std::cout << "\n";


    std::cout << "Preparing data\n";

    // Read first line that is file descriptor
    getline(in_file, line);

    // Load all kMers
    while(getline(in_file, line)) {
        kmer += line;
        while (kmer.length() >= k) {
            to_add.push_back(kmer.substr(0, k));
            kmer.erase(0, 1);
        }
    }
    for (std::string& s : to_add) {
        to_add_bytes.push_back(std::as_bytes(std::span(s)));
    }
    
    // Generating random kmers for lookup
    for (int i = 0; i < num_operations; i++) {
        kmer = "";
        for (int j = 0; j < k; j++) {
            kmer += bases[distribution(generator)];
        }
        to_lookup.push_back(kmer);
    }
    for (std::string& s : to_lookup) {
        to_lookup_bytes.push_back(std::as_bytes(std::span(s)));
    }
    
    std::cout << "\tNum of insert elements: " << to_add.size() << "\n";
    std::cout << "\tNum of lookup elements: " << to_lookup.size() << "\n";
    std::cout << "Initial capacity: " << (1u << (int)ceil(log2(to_add_bytes.size()))) << "\n";
    std::cout << "\n";

    BambooFilter* bf = new BambooFilter(to_add_bytes.size());

    // Start timer--Insert
    std::cout << "Inserting genome\n";
    auto start_time = NowNanos();

    for (auto& element : to_add_bytes) {
        bf->Insert(element);
    }

    auto insert_rate = ((to_add_bytes.size() * 1000.0) / static_cast<double>(NowNanos() - start_time));
    std::cout << "Insert rate: " << insert_rate << "\n";

    // Start timer--Lookup
    std::cout << "Looking for random kMers\n";
    start_time = NowNanos();

    int pos = 0, neg = 0;

    for (auto& element : to_lookup_bytes) {
        if (bf->Lookup(element)) pos++;
        else neg++;
    }

    auto lookup_rate = ((to_lookup_bytes.size() * 1000.0) / static_cast<double>(NowNanos() - start_time));
    std::cout << "Lookup rate: " << lookup_rate << "\n";

    std::cout << "Positive finds: " << pos << "\n";
    std::cout << "Negative finds: " << neg << "\n";

    size_t count_found_real = 0;
    std::sort(to_add.begin(), to_add.end());
    std::sort(to_lookup.begin(), to_lookup.end());

    auto it = to_add.begin();
    for (auto& elem : to_lookup) {
        while (*it < elem && it != to_add.end()) {
            it++;
        }
        if (*it == elem) {
            count_found_real++;
        }
    }

    double false_positive_rate = (pos - count_found_real) / (double)num_operations;

    std::cout << "False positive rate: " << false_positive_rate << '\n';

    out_file << "# k_mer;operations;insert_rate;lookup_rate;false_positive_rate\n";
    out_file << k << ';';
    out_file << to_lookup_bytes.size() << ';';
    out_file << insert_rate << ';';
    out_file << lookup_rate << ';';
    out_file << false_positive_rate << '\n';
    
    delete bf;

    return 0;
}