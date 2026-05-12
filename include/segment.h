#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>

class Segment {
    public:
        Segment(/*TODO*/);

        ~Segment();

        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);
        bool Lookup(/*TODO*/) const;
        bool Delete(/*TODO*/);
        // TODO: Probably needs more public functions

    private:
        // Internal helper functions
        [[nodiscard]] inline uint32_t get_other_bucket(uint32_t index_bucket, uint32_t fingerprint); 
        inline bool insert_in_bucket(uint32_t fingerprint, uint32_t index_bucket);
        [[nodiscard]] inline uint32_t swap_with_random_in_bucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);

        // Constants
        static const std::size_t kMaxEvictions_;

        // Attributes
        std::vector<std::vector<uint32_t>> buckets_; // TODO: Can be flattened
};

#endif