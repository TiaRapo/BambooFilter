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

void BambooFilter::CalculateIndices(std::span<const std::byte> elem, uint32_t &fingerprint, uint32_t &index_bucket, uint32_t &index_segment) const {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    fingerprint = (hash >> kNumBitsInitialTable_) & kMaskFingerprint;
    index_bucket = hash & kMaskBucket;
    index_segment = (hash >> kNumBitsBucket) & (num_bits_table_ - kNumBitsBucket);
}

// TODO: Test
bool BambooFilter::Insert(std::span<const std::byte> elem) {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

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
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    if (segments_[index_segment]->Lookup(fingerprint, index_bucket)) return true;   // Lookup current segment

    if (segments_[index_segment]->GetOverflow()) return segments_[index_segment]->GetOverflow()->Lookup(fingerprint, index_bucket); // Lookup overflow segment

    return false;
}

bool BambooFilter::Delete(std::span<const std::byte> elem) {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    bool elem_found = segments_[index_segment]->Lookup(fingerprint, index_bucket);
    bool deleted = false;

    if (elem_found) {
        deleted = segments_[index_segment]->Delete(fingerprint, index_bucket);    // Delete from found bucket

        if (!deleted && segments_[index_segment]->GetOverflow()) deleted = segments_[index_segment]->Delete(fingerprint, index_bucket); // Delete from overflow

        if (deleted) num_elems_--;

        if (!(num_elems_ & (kResizingThreshold - 1))) { // If num_elems_ is a multiple of threshold
        Compress();
    }
    }

    return deleted;
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