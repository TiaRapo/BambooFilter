#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>
#include <array>

#include "config.h"
#include "bucket.h"

class Segment {
    public:
        // Constructors & Destructors
        Segment() = default;
        Segment(Segment* original);
        ~Segment();

        // Getters
        [[nodiscard]] const std::array<Bucket, kBucketsPerSegment>& GetBuckets() const noexcept;
        [[nodiscard]] Segment* GetOverflow() const noexcept;

        // Operations
        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng);
        bool Lookup(uint32_t fingerprint, uint32_t index_bucket) const;
        bool Delete(uint32_t fingerprint, uint32_t index_bucket);
        void EraseByBit(bool bit_value, uint32_t bit_index);
        bool MergeSegment(Segment* other, std::mt19937& rng);
        
    private:
        // Internal helper functions
        inline bool InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937& rng);
        inline void AddOverflow();
        [[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const;

        // Attributes
        std::array<Bucket, kBucketsPerSegment> buckets_;
        Segment* overflow_;
};

#endif