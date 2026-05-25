#include "bamboo_filter.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <random>
#include <vector>
#include <memory>
#include <utility>

#include "config.h"
#include "segment.h"
#include "wyhash.h"

// Ivan & Tia
BambooFilter::BambooFilter(uint32_t initial_capacity)
        : kNumBitsInitialTable_{std::max((size_t)ceil(log2(static_cast<double>(initial_capacity) / (double)kFingerprintsPerBucket)), kNumBitsBucket + 1)},
        kSeed_{static_cast<uint32_t>(std::random_device{}())},
        segments_(size_t{1} << (kNumBitsInitialTable_ - kNumBitsBucket)),
        num_bits_table_{kNumBitsInitialTable_},
        num_elems_{0},
        index_split_sgm_{0u},
        rng_{kSeed_} {}

// Ivan
BambooFilter::~BambooFilter() {
    for (auto* s : segments_) {
        delete s;
    }
}

// Ivan
[[nodiscard]] size_t BambooFilter::GetNumElems() const noexcept {
    return num_elems_;
}

// Ivan
bool BambooFilter::Insert(std::span<const std::byte> elem) {
    uint32_t fingerprint, index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    segments_[index_segment]->Insert(fingerprint, index_bucket, rng_);
    num_elems_++;
    
    if (!(num_elems_ & (kResizingThreshold - 1))) {
        Expand();
    }

    return true;
}

// Tia
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

// Ivan & Tia
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
        if (!(num_elems_ & (kResizingThreshold - 1))) {
            Compress();
        }

        num_elems_--;
    }

    return deleted;
}

// Tia
[[nodiscard]] uint32_t BambooFilter::GetCapacity() const noexcept {
    uint32_t total_cap = segments_.size() * kBucketsPerSegment * kFingerprintsPerBucket;
    return total_cap;
}

// Tia
void BambooFilter::Expand() { // TODO: CHECK IF THIS IS GOOD? Erase by bit first?
    // Copy the splitting segment and add it to the filter
    Segment* orig_segment = segments_[index_split_sgm_];
    Segment* splt_segment = new Segment(orig_segment);
    segments_.push_back(splt_segment);

    // Collect all bucket_index-fingerprint pairs from the overflow segments 
    std::vector<std::pair<uint32_t, uint32_t>> overflow_elements;
    Segment* overflow_segment = orig_segment->GetOverflow();
    while (overflow_segment) {
        std::array<Bucket, kBucketsPerSegment> overflow_buckets = overflow_segment->GetBuckets();
        for (int index = 0 ; index < overflow_buckets.size() ; index++) {
            for (uint8_t j = 0 ; j < overflow_buckets[index].GetSize() ; j++) {
                overflow_elements.push_back({index, overflow_buckets[index].GetEntries()[j]});
            }
        }
        overflow_segment = overflow_segment->GetOverflow();
    }

    delete orig_segment->GetOverflow();

    orig_segment->EraseByBit(1, num_bits_table_); // Remove entries where the newly extended segment bit is 1
    splt_segment->EraseByBit(0, num_bits_table_); // Remove entries where the newly extended segment bit is 0

    // Return overflow elements to matching segments
    for (std::pair<uint32_t, uint32_t> element : overflow_elements) {
        if (element.second & uint32_t{1} << num_bits_table_ != uint32_t{0}) {
            splt_segment->Insert(element.second, element.first, rng_);
        } else {
            orig_segment->Insert(element.second, element.first, rng_);
        }
    }

    index_split_sgm_++;

    if (index_split_sgm_ == (1u << (int)ceil(log2(segments_.size())))) {
        index_split_sgm_ = 0u;
        num_bits_table_++;
    }
}

// Ivan
void BambooFilter::Compress() {
    if (index_split_sgm_ == 0u) {
        index_split_sgm_ = (uint32_t{1} << (int)ceil(log2(segments_.size()))) - 1u;
        num_bits_table_--;
    } else {
        index_split_sgm_--;
    }

    Segment* segment_dst = segments_[index_split_sgm_];
    Segment* segment_src = segments_.back();

    uint32_t round = num_bits_table_ - kNumBitsInitialTable_;

    segment_dst->MergeSegment(segment_src, rng_);
    segments_.pop_back();
    delete segment_src; 
}

// Ivan
inline void BambooFilter::CalculateIndices(
    std::span<const std::byte> elem,
    uint32_t& fingerprint,
    uint32_t& index_bucket,
    uint32_t& index_segment
) const {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    fingerprint = (hash >> (sizeof(hash) * 8 - kNumBitsFingerprint)) & kMaskFingerprint;
    index_bucket = hash & kMaskBucket;
    index_segment = (hash >> kNumBitsBucket) & ((uint32_t{1} << (num_bits_table_ - kNumBitsBucket)) - 1u);
}