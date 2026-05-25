#include "segment.h"

#include <cstdint>
#include <random>
#include <vector>
#include <memory>
#include <algorithm>

#include "utility.h"
#include "config.h"

inline bool Bucket::Insert(uint32_t fingerprint) {
    if (size_ < kFingerprintsPerBucket) {
        entries_[size_] = fingerprint;
        size_++;
        return true;
    }

    return false;
}

bool Bucket::Delete(uint32_t fingerprint) {
    for (uint8_t i = 0 ; i < size_ ; i++) {
        if (entries_[i] == fingerprint) {
            entries_[i] = entries_[size_ - 1];
            size_--;
            return true;
        }
    }

    return false;
}

bool Bucket::Lookup(uint32_t fingerprint) const {
    for (uint8_t i = 0 ; i < size_ ; i++) {
        if (entries_[i] == fingerprint) {
            return true;
        }
    }

    return false;
}

void Bucket::EraseByBit(bool bit_value, uint32_t bit_index) {
    for (uint8_t i = size_ - 1 ; i >= 0 ; i--) {
        if (((entries_[i] & (uint32_t{1} << bit_index)) != uint32_t{0}) == bit_value) {
            entries_[i] = entries_[size_ - 1];
            size_--;
        }
    }
}

[[nodiscard]] inline uint32_t Bucket::SwapWithRandom(uint32_t fingerprint, std::mt19937 &rng) {
    uint32_t entry_index = rng() % kFingerprintsPerBucket;
    uint32_t taken = entries_[entry_index];
    entries_[entry_index] = fingerprint;
    return taken;
}

// Ivan & Tia
// std::ostream& operator<<(std::ostream& os, const Segment& s) {
//     for (size_t i = 0 ; i < s.buckets_.size() ; i++) {
//         std::vector<uint32_t> bucket = s.buckets_[i];
//         if (bucket.empty()) continue;
//         os << "\tBucket " << i << ':';
//         for (uint32_t element : bucket) {
//             os << ' ' << element;
//         }
//         os << "\n";
//     }

//     return os;
// }

// Ivan & Tia
Segment::Segment()
        : buckets_{},
        overflow_(nullptr) {}

// Tia
Segment::Segment(Segment* original)
        : buckets_(original->buckets_),
        overflow_(nullptr) {}

// Ivan & Tia
[[nodiscard]] Segment* Segment::GetOverflow() const noexcept {
    return overflow_;
}

// Ivan & Tia
bool Segment::Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng) {
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    Segment* segment = this;
    
    while (true) {
        if (segment->InsertLocal(fingerprint, index_bucket, index_bucket_other, rng)) return true;

        Segment* overflow = segment->GetOverflow();
        if (overflow == nullptr) {
            segment->AddOverflow();
            segment = segment->GetOverflow();
            segment->InsertLocal(fingerprint, index_bucket, index_bucket_other, rng);
            return true;
        }

        segment = overflow;
    }

    return false;
}

// Ivan
bool Segment::MergeSegment(Segment& other, std::mt19937& rng) {
    for (size_t bucket_index = 0 ; bucket_index < kBucketsPerSegment ; bucket_index++) {
        for (uint8_t i = 0 ; i < other.buckets_[bucket_index].size_ ; i++) {
            Insert(other.buckets_[bucket_index].entries_[i], bucket_index, rng);
        }
    }

    return true;
}

// Ivan & Tia
bool Segment::Lookup(uint32_t fingerprint, uint32_t index_bucket) const {
    if (buckets_[index_bucket].Lookup(fingerprint)) {
        return true;
    }

    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    if (buckets_[index_bucket_other].Lookup(fingerprint)) {
        return true;
    }

    return false;
}

// Ivan
bool Segment::Delete(uint32_t fingerprint, uint32_t index_bucket) {
    if (buckets_[index_bucket].Delete(fingerprint)) {
        return true;
    }

    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    if (buckets_[index_bucket_other].Delete(fingerprint)) {
        return true;
    }

    return false;
}

// Ivan & Tia
void Segment::EraseByBit(bool bit_value, uint32_t bit_index) {
    for (auto &bucket : buckets_) {
       bucket.EraseByBit(bit_value, bit_index);
    }
}

// Ivan & Tia
bool Segment::InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937 &rng) {
    if (buckets_[index_bucket].Insert(fingerprint) || buckets_[index_bucket_other].Insert(fingerprint)) {
        return true;
    }

    if (rng() & uint32_t{1}) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (size_t i = 0 ; i < kMaxEvictions ; i++) {
        fingerprint = buckets_[index_bucket].SwapWithRandom(fingerprint, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (buckets_[index_bucket].Insert(fingerprint)) {
            return true;
        }
    }

    return false;
}

// Ivan
void Segment::AddOverflow() {
    overflow_ = new Segment();
}

// Ivan
[[nodiscard]] inline uint32_t Segment::GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const {
    return (index_bucket ^ fingerprint) & kMaskBucket;
}