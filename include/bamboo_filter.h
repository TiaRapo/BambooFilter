#ifndef BAMBOOFILTER_H
#define BAMBOOFILTER_H

#include <cstddef>
#include <cstdint>
#include <span>
#include <random>
#include <vector>
#include <memory>

#include "segment.h"

class BambooFilter {
    public:
        BambooFilter(uint32_t capacity /*TODO*/);

        ~BambooFilter();

        std::size_t GetNumElems() const noexcept;

        bool Insert(std::span<const std::byte> elem);
        bool Lookup(std::span<const std::byte> elem) const;
        bool Delete(std::span<const std::byte> elem);
        bool Expand();
        void Compress();

    private:
        // TODO: Internal helper functions ?

        // Constants
        const std::size_t kNumBitsInitialTable_; // Bucket bit count + initial segment bit count
        const std::uint32_t kSeed_;

        // Attributes
        std::vector<Segment*> segments_;    // Segments
        std::size_t num_bits_table_;        // Bucket bit count + segment bit count
        std::size_t num_elems_;             // Number of elements
        std::uint32_t index_split_sgm_;     // Index of the segment that will be splitted next

        // Helpers
        std::mt19937 rng_;
};

#endif