#ifndef SEGMENT_H
#define SEGMENT_H

#include <cstdint>
#include <random>
#include <vector>
#include <memory>

class Segment {
    public:
        Segment(/*TODO*/);
        Segment(Segment* original);

        ~Segment();

        [[nodiscard]] Segment* GetOverflow() noexcept;

        bool Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);
        bool Lookup(/*TODO*/) const;
        bool Delete(/*TODO*/);
        // TODO: Probably needs more public functions
        bool EraseByBit(bool bit_value, std::uint32_t bit_index);

    private:
        // Internal helper functions
        [[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const; 
        inline bool InsertInBucket(uint32_t fingerprint, uint32_t index_bucket);
        [[nodiscard]] inline uint32_t SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng);

        // Constants
        static const std::size_t kMaxEvictions_;

        // Attributes
        std::vector<std::vector<uint32_t>> buckets_; // TODO: Can be flattened
        std::unique_ptr<Segment> overflow_;
};

#endif