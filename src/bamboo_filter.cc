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
        kNumBitsInitialTable_((std::uint32_t)ceil(log2(static_cast<double>(capacity) / 4.0))), // TODO: Explain this
        num_elems_(0),
        index_split_sgm_(0) {
    // Placing the initialization of variables that depend on the initialization of other variables in constructor body ...
    // ... because it depends on the declaration order in class which is risky
    num_bits_table_ = kNumBitsInitialTable_;
    rng_.seed(kSeed_);
}

BambooFilter::~BambooFilter() {
    for (Segment* s : segments_) {
        delete s;
    }
}

[[nodiscard]] std::size_t BambooFilter::GetNumElems() const noexcept {
    return num_elems_;
}

bool BambooFilter::Insert(std::span<const std::byte> elem) {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    Segment* segment = segments_[index_segment];

    while (true) {
        if (segment->Insert(fingerprint, index_bucket, rng_)) {
            break;
        }

        Segment* overflow = segment->GetOverflow();
        if (overflow == nullptr) {
            segment->AddOverflow();
            segment = segment->GetOverflow();
            segment->Insert(fingerprint, index_bucket, rng_);
            break;
        }

        segment = overflow;
    }

    num_elems_++;
    
    if (!(num_elems_ & (kResizingThreshold - 1))) { // If num_elems_ is a multiple of threshold
        Expand();
    }

    return true;
}

bool BambooFilter::Lookup(std::span<const std::byte> elem) const {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    for (Segment* segment = segments_[index_segment] ; segment != nullptr ; segment = segment->GetOverflow()) {
        if (segment->Lookup(fingerprint, index_bucket)) {
            return true;
        }
    }

    return false;
}

bool BambooFilter::Delete(std::span<const std::byte> elem) {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    bool deleted = false;

    Segment* segment = segments_[index_segment];

    for (Segment* segment = segments_[index_segment] ; segment != nullptr ; segment = segment->GetOverflow()) {
        if (segment->Delete(fingerprint, index_bucket)) {
            deleted = true;
            break;
        }
    }

    if (deleted) {
        if (!(num_elems_ & (kResizingThreshold - 1))) { // If num_elems_ is a multiple of threshold
            Compress();
        }

        num_elems_--;
    }

    return deleted;
}

void BambooFilter::Expand() {
    // Copy the splitting segment and add it to the filter
    Segment* orig_segment = segments_[index_split_sgm_];
    Segment* splt_segment = new Segment(orig_segment);
    segments_.push_back(splt_segment);

    std::uint32_t round = num_bits_table_ - kNumBitsInitialTable_;

    orig_segment->EraseByBit(1, round);     // Remove entries where i-th bit is 1
    splt_segment->EraseByBit(0, round);     // Remove entries where i-th bit is 0

    index_split_sgm_++;

    if (index_split_sgm_ == (pow(2, ceil(log2(segments_.size()))))) {
        index_split_sgm_ = 0;
    }
}

void BambooFilter::Compress() {
    if (index_split_sgm_ == 0) {
        index_split_sgm_ = (pow(2, ceil(log2(segments_.size())))) - 1;
    } else {
        index_split_sgm_--;
    }

    Segment* segment_dst = segments_[index_split_sgm_];
    Segment* segment_src = segments_.back();

    std::uint32_t round = num_bits_table_ - kNumBitsInitialTable_;

    segment_dst->Insert(*segment_src);
    segments_.pop_back();
    delete segment_src; 
}

void BambooFilter::CalculateIndices(std::span<const std::byte> elem, uint32_t &fingerprint, uint32_t &index_bucket, uint32_t &index_segment) const {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    fingerprint = (hash >> kNumBitsInitialTable_) & kMaskFingerprint;
    if (fingerprint = kEmptyFingerprint) {
        fingerprint = kEmptyFingerprintReplacement; // TODO: Do we want a better replacement method?
    }
    index_bucket = hash & kMaskBucket;
    index_segment = (hash >> kNumBitsBucket) & (num_bits_table_ - kNumBitsBucket);
}