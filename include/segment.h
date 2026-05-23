#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>
#include <memory>
#include <iostream>
#include <string>

class Segment {
    friend std::ostream& operator<<(std::ostream& os, const Segment& s);
    
    public:
        Segment(/*TODO?*/);
        Segment(Segment* original);

        ~Segment();

        [[nodiscard]] Segment* GetOverflow() const noexcept;
        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng);
        bool Insert(Segment& other, std::mt19937& rng);
        bool Lookup(uint32_t fingerprint, uint32_t index_bucket) const;
        bool Delete(uint32_t fingerprint, uint32_t index_bucket);
        void EraseByBit(bool bit_value, std::uint32_t bit_index);
        
    private:
        // Internal helper functions
        inline bool InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937& rng);
        void AddOverflow();
        [[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const; 
        inline bool InsertInBucket(uint32_t fingerprint, uint32_t index_bucket);
        [[nodiscard]] inline uint32_t SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng);

        // Attributes
        std::vector<std::vector<uint32_t>> buckets_; // TODO: Can be flattened (and buckets can be pre-chained)
        Segment* overflow_;
};

#endif