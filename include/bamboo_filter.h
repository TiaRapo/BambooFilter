#ifndef BAMBOOFILTER_H
#define BAMBOOFILTER_H

#include <cstddef>
#include <cstdint>
#include <span>
#include <random>
#include <vector>

#include "config.h"
#include "segment.h"

class BambooFilter {
    public:
        // Constructors & Destructors
        BambooFilter(uint32_t initial_capacity);
        ~BambooFilter();

        // Getters
        [[nodiscard]] size_t GetNumElems() const noexcept;
        [[nodiscard]] uint32_t GetCapacity() const noexcept;

        // Operations
        bool Insert(std::span<const std::byte> elem);
        bool Lookup(std::span<const std::byte> elem) const;
        bool Delete(std::span<const std::byte> elem);

    private:
        // Internal helper functions
        void Expand();
        void Compress();
        inline void CalculateIndices(
            std::span<const std::byte> elem,
            uint32_t& fingerprint,
            uint32_t& index_bucket,
            uint32_t& index_segment
        ) const;

        // Constants
        const size_t kNumBitsInitialTable_; // Bucket bit count + initial segment bit count
        const uint32_t kSeed_; // Seed for the RNG

        // Attributes
        std::vector<Segment*> segments_; // Segments
        size_t num_bits_table_; // Bucket bit count + segment bit count
        size_t num_elems_; // Number of elements
        uint32_t index_split_sgm_; // Index of the segment that will be split next

        // Helpers
        std::mt19937 rng_;
};

#endif