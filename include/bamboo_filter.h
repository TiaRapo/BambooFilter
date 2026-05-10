#ifndef BAMBOOFILTER_H
#define BAMBOOFILTER_H

#include <cstddef>
#include <cstdint>
#include <span>
#include <random>

#include "segment.h"

class BambooFilter {
    public:
        BambooFilter(uint32_t capacity /*TODO*/);

        ~BambooFilter();

        bool Insert(std::span<const std::byte> elem);
        bool Lookup(std::span<const std::byte> elem) const;
        bool Delete(std::span<const std::byte> elem);
        void Extend();
        void Compress();

    private:
        // TODO: Internal helper functions ?

        const std::uint32_t kSeed_;
        const std::size_t kNumBitsInitialTable_; // Bucket bit count + initial segment bit count

        std::size_t num_bits_table_;

        // TODO: More attributes
};

#endif