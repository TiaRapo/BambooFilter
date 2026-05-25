#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>
#include <memory>
#include <iostream>
#include <string>
#include <array>

#include "config.h"

class Bucket {
    friend class Segment;

    private:
        inline bool Insert(uint32_t fingerprint);
        bool Delete(uint32_t fingerprint);
        bool Lookup(uint32_t fingerprint) const;
        void EraseByBit(bool bit_value, uint32_t bit_index);
        [[nodiscard]] inline uint32_t SwapWithRandom(uint32_t fingerprint, std::mt19937 &rng);

        uint8_t size_ = 0;
        std::array<uint32_t, kFingerprintsPerBucket> entries_{};
};

class Segment {
    // friend std::ostream& operator<<(std::ostream& os, const Segment& s);
    
    public:
        Segment(/*TODO?*/);
        Segment(Segment* original);

        [[nodiscard]] Segment* GetOverflow() const noexcept;
        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng);
        bool MergeSegment(Segment& other, std::mt19937& rng);
        bool Lookup(uint32_t fingerprint, uint32_t index_bucket) const;
        bool Delete(uint32_t fingerprint, uint32_t index_bucket);
        void EraseByBit(bool bit_value, uint32_t bit_index);
        
    private:
        // Internal helper functions
        inline bool InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937& rng);
        void AddOverflow();
        [[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const;

        // Attributes
        std::array<Bucket, kBucketsPerSegment> buckets_;
        Segment* overflow_;
};

#endif