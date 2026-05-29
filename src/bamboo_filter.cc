#include "bamboo_filter.h"

#include <cstddef>
#include <cstdint>
#include <span>
#include <random>
#include <vector>
#include <memory>
#include <utility>
#include <limits>

#include "config.h"
#include "segment.h"
#include "wyhash.h"

// Ivan & Tia
BambooFilter::BambooFilter(size_t initial_capacity)
        : kNumBitsInitialTable_{
            kNumBitsBucket +
            static_cast<size_t>(
                std::max(
                    ceil(log2(
                        static_cast<double>(initial_capacity) /
                        static_cast<double>(kFingerprintsPerBucket) /
                        static_cast<double>(kBucketsPerSegment)
                    )),
                    1.0
                )
            ),
        },
        kSeed_{static_cast<uint32_t>(std::random_device{}())},
        segments_(size_t{1} << (kNumBitsInitialTable_ - kNumBitsBucket)),
        num_bits_table_{kNumBitsInitialTable_ + 1}, // Want to be using the segment overlap with fingerprint immediately
        num_elems_{0},
        index_split_sgm_{0},
        rng_{kSeed_} {
    for (auto& s : segments_) {
        s = new Segment(); // They are nullptr initially
    }
}

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

// Tia
[[nodiscard]] uint32_t BambooFilter::GetCapacity() const noexcept {
    uint32_t total_cap = segments_.size() * kBucketsPerSegment * kFingerprintsPerBucket;
    return total_cap;
}

// Ivan
bool BambooFilter::Insert(std::span<const std::byte> elem) {
    fingerprint_t fingerprint;
    uint32_t index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    segments_[index_segment]->Insert(fingerprint, index_bucket, rng_);
    num_elems_++;
    
    if (!(num_elems_ & (kResizingThreshold - 1))) { // num_elems_ is multiple of kResizingThreshold
        Expand();
    }

    return true;
}

// Tia
bool BambooFilter::Lookup(std::span<const std::byte> elem) const {
    fingerprint_t fingerprint;
    uint32_t index_bucket, index_segment;
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
    fingerprint_t fingerprint;
    uint32_t index_bucket, index_segment;
    CalculateIndices(elem, fingerprint, index_bucket, index_segment);

    bool is_deleted = false;

    Segment* segment = segments_[index_segment];

    for (Segment* segment = segments_[index_segment] ; segment != nullptr ; segment = segment->GetOverflow()) {
        if (segment->Delete(fingerprint, index_bucket)) {
            is_deleted = true;
            break;
        }
    }

    if (is_deleted) {
        if (!(num_elems_ & (kResizingThreshold - 1))) { // num_elems_ falling below a multiple kResizingThreshold
            Compress();
        }

        num_elems_--;
    }

    return is_deleted;
}

// Tia
void BambooFilter::Expand() {
    // Copy the splitting segment and add it to the filter
    Segment* orig_segment = segments_[index_split_sgm_];
    Segment* splt_segment = new Segment(orig_segment);
    segments_.push_back(splt_segment);

    // Collect all bucket_index-fingerprint pairs from the overflow segments 
    std::vector<std::pair<uint32_t, fingerprint_t>> overflow_elements;
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

    orig_segment->ClearOverflow();

    uint32_t round = num_bits_table_ - kNumBitsInitialTable_ - 1;
    orig_segment->EraseByBit(1, round); // Remove entries where the leftmost segment bit is 1
    splt_segment->EraseByBit(0, round); // Remove entries where the leftmost segment bit is 0

    // Return overflow elements to matching segments
    for (std::pair<uint32_t, fingerprint_t> element : overflow_elements) {
        if ((element.second & (fingerprint_t{1} << round)) != fingerprint_t{0}) {
            splt_segment->Insert(element.second, element.first, rng_);
        } else {
            orig_segment->Insert(element.second, element.first, rng_);
        }
    }

    uint32_t round_size = uint32_t{1} << (num_bits_table_ - kNumBitsBucket - uint32_t{1});
    index_split_sgm_++;
    if (index_split_sgm_ == round_size) {
        index_split_sgm_ = 0;
        num_bits_table_++;
    }
}

// Ivan
void BambooFilter::Compress() {
    if (index_split_sgm_ == 0) {
        num_bits_table_--;
        index_split_sgm_ = (uint32_t{1} << (num_bits_table_ - kNumBitsBucket - uint32_t{1})) - uint32_t{1};
    } else {
        index_split_sgm_--;
    }

    Segment* segment_dst = segments_[index_split_sgm_];
    Segment* segment_src = segments_.back();

    segment_dst->MergeSegment(segment_src, rng_);
    segments_.pop_back();
    delete segment_src; 
}

// Ivan
inline void BambooFilter::CalculateIndices(
    std::span<const std::byte> elem,
    fingerprint_t& fingerprint,
    uint32_t& index_bucket,
    uint32_t& index_segment
) const {
    uint32_t hash = wyhash(elem.data(), elem.size(), kSeed_, _wyp);

    fingerprint = (hash >> (kNumBitsInitialTable_)) & kMaskFingerprint;
    index_bucket = hash & kMaskBucket;
    index_segment = (hash >> kNumBitsBucket) & ((uint32_t{1} << (num_bits_table_ - kNumBitsBucket)) - uint32_t{1});
    if (index_segment >= segments_.size()) {
        index_segment ^= (uint32_t{1} << (num_bits_table_ - kNumBitsBucket - 1));
    }
}