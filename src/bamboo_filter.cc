#include "bamboo_filter.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <cmath>

#include "config.h"
#include "segment.h"
#include "utility.h"
#include "wyhash.h"

BambooFilter::BambooFilter(uint32_t capacity /*TODO*/) // TODO
        : kSeed_(static_cast<std::uint32_t>(std::random_device{}())),
        kNumBitsInitialTable_((std::uint32_t)ceil(log2(static_cast<double>(capacity) / 4.0))) {
    // Placing the initialization of variables that depend on the initialization of other variables in constructor body ...
    // ... because it depends on the declaration order in class which is risky
    num_bits_table_ = kNumBitsInitialTable_;
    rng_.seed(kSeed_);
}

BambooFilter::~BambooFilter() {
    // TODO ?
}

bool BambooFilter::Insert(std::span<const std::byte> elem) {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    uint32_t fingerprint = (hash >> kNumBitsInitialTable_) & kMaskFingerprint;
    uint32_t index_bucket = hash & kMaskBucket;
    uint32_t index_bucket_other = (index_bucket ^ fingerprint) & kMaskBucket;
    uint32_t index_segment = (hash >> kNumBitsBucket) & (num_bits_table_ - kNumBitsBucket);

    bool is_inserted = false;

    if (!segments_[index_segment]->Insert(fingerprint, index_bucket) &&
        !segments_[index_segment]->Insert(fingerprint, index_bucket_other)) { // Couldn't find an empty entry in either bucket of segment
        if (rng_() & 1u) { // 50% chance to pick each of the two buckets
            index_bucket = index_bucket_other;
        }

        for (std::size_t i = 0 ; i < max_evictions_ ; i++) {
            // Randomly select an entry e from bucket
            // Swap our fingerprint with the one in that entry
            // Calculate other bucket of the swapped fingerprint
            // If that bucket has an empty entry:
                // Store f there
                // is_inserted = true
                // Break
            // Swap buckets
        }
    } else {
        is_inserted = true;
    }

    if (!is_inserted) {
        // segments_[index_segment]->overflow.Insert(fingerprint, index_bucket);
    }

    if (false /*TODO: expand condition*/) {
        Expand();
    }

    return true;
}

bool BambooFilter::Lookup(std::span<const std::byte> elem) const {
    // TODO
}

bool BambooFilter::Delete(std::span<const std::byte> elem) {
    // TODO
}

void BambooFilter::Expand() {
    // TODO
}

void BambooFilter::Compress() {
    // TODO
}