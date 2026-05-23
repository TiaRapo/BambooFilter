#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>
#include <memory>

class Segment {
    public:
        Segment(/*TODO?*/);
        Segment(Segment* original);

        ~Segment();

        [[nodiscard]] Segment* GetOverflow() const noexcept;
        void AddOverflow();
        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);
        bool Insert(Segment& other);
        bool Lookup(uint32_t fingerprint, uint32_t index_bucket) const;
        bool Delete(uint32_t fingerprint, uint32_t index_bucket);
        bool EraseByBit(bool bit_value, std::uint32_t bit_index);

    private:
        // Internal helper functions
        [[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const; 
        inline bool InsertInBucket(uint32_t fingerprint, uint32_t index_bucket);
        [[nodiscard]] inline uint32_t SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);

        // Attributes
        std::vector<std::vector<uint32_t>> buckets_; // TODO: Can be flattened
        Segment* overflow_;
};

#endif