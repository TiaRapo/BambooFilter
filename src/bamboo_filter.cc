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
        kNumBitsInitialTable_((std::uint32_t)ceil(log2(static_cast<double>(capacity) / 4.0))), 
        num_elems_(0),
        index_split_sgm_(0) {
    // Placing the initialization of variables that depend on the initialization of other variables in constructor body ...
    // ... because it depends on the declaration order in class which is risky
    num_bits_table_ = kNumBitsInitialTable_;
    rng_.seed(kSeed_);
}

BambooFilter::~BambooFilter() {
    // TODO ?
}

std::size_t BambooFilter::GetNumElems() const noexcept {
    return num_elems_;
}

// TODO: Test
bool BambooFilter::Insert(std::span<const std::byte> elem) {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    uint32_t fingerprint = (hash >> kNumBitsInitialTable_) & kMaskFingerprint;
    uint32_t index_bucket = hash & kMaskBucket;
    uint32_t index_segment = (hash >> kNumBitsBucket) & (num_bits_table_ - kNumBitsBucket);

    if (!segments_[index_segment]->Insert(fingerprint, index_bucket, rng_)) { // Couldn't find free spot for entry
        segments_[index_segment]->GetOverflow()->Insert(fingerprint, index_bucket, rng_); // TODO: Should it recursively go down?
    }

    if (!(num_elems_ & (kResizingThreshold - 1))) { // If num_elems_ is a multiple of threshold
        Expand();
    }

    num_elems_++;
    return true;
}

bool BambooFilter::Lookup(std::span<const std::byte> elem) const {
    // TODO
}

bool BambooFilter::Delete(std::span<const std::byte> elem) {
    // TODO
}

bool BambooFilter::Expand() {
    // Copy the splitting segment and add it to the filter
    Segment* orig_segment = segments_[index_split_sgm_];
    Segment* splt_segment = new Segment(orig_segment);
    segments_.push_back(splt_segment);

    std::uint32_t round = num_bits_table_ - kNumBitsInitialTable_;

    orig_segment->EraseByBit(1, round);     // Remove entries where i-th bit is 1
    splt_segment->EraseByBit(0, round);     // Remove entries where i-th bit is 0

    index_split_sgm_++;

    if (index_split_sgm_ == (pow(2, ceil(log2(segments_.size()))))) index_split_sgm_ = 0;
    return true;
}

void BambooFilter::Compress() {
    // TODO
}