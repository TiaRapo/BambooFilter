#include "segment.h"

#include <cstdint>
#include <random>
#include <vector>
#include <memory>
#include <algorithm>

#include "utility.h"
#include "config.h"

// Ivan & Tia
std::ostream& operator<<(std::ostream& os, const Segment& s) {
    for (std::vector<uint32_t> bucket : s.buckets_) {
        if (bucket.empty()) continue;
        for (uint32_t element : bucket) {
            os << element << " ";
        }
        os << "\n";
    }

    return os;
}

// Ivan & Tia
Segment::Segment()
        : buckets_(2 << kNumBitsBucket, std::vector<uint32_t>()),
        overflow_(nullptr) {
    for (std::vector<uint32_t> bucket : buckets_) {
        bucket.reserve(4);
    }
}

// Tia
Segment::Segment(Segment* original)
        : buckets_(2 << kNumBitsBucket, std::vector<uint32_t>()),
        overflow_(nullptr) {
    buckets_ = original->buckets_;
}

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
        for (uint32_t fingerprint : other.buckets_[bucket_index]) Insert(fingerprint, bucket_index, rng);
    }

    return true;
}

// Ivan & Tia
bool Segment::Lookup(uint32_t fingerprint, uint32_t index_bucket) const {
    auto it = std::find(buckets_[index_bucket].begin(), buckets_[index_bucket].end(), fingerprint);
    if (it != buckets_[index_bucket].end()) return true;
    
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    it = std::find(buckets_[index_bucket_other].begin(), buckets_[index_bucket_other].end(), fingerprint);
    if (it != buckets_[index_bucket_other].end()) return true;

    return false;
}

// Ivan
bool Segment::Delete(uint32_t fingerprint, uint32_t index_bucket) {
    auto it = std::find(buckets_[index_bucket].begin(), buckets_[index_bucket].end(), fingerprint);
    if (it != buckets_[index_bucket].end()) {
        buckets_[index_bucket].erase(it);
        return true;
    }

    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);
    
    it = std::find(buckets_[index_bucket_other].begin(), buckets_[index_bucket_other].end(), fingerprint);
    if (it != buckets_[index_bucket_other].end()) {
        buckets_[index_bucket_other].erase(it);
        return true;
    }

    return false;
}

// Ivan & Tia
void Segment::EraseByBit(bool bit_value, uint32_t bit_index) {
    for (auto &bucket : buckets_) {
        bucket.erase(
            std::remove_if(bucket.begin(), bucket.end(),
                [bit_value, bit_index](uint32_t entry) {
                    return ((entry & (uint32_t{1} << bit_index)) != uint32_t{0}) == bit_value;
                }),
            bucket.end()
        );
    }
}

// Ivan & Tia
bool Segment::InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937 &rng) {
    if (InsertInBucket(fingerprint, index_bucket) || InsertInBucket(fingerprint, index_bucket_other)) {
        return true;
    }

    if (rng() & uint32_t{1}) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (size_t i = 0 ; i < kMaxEvictions ; i++) {
        fingerprint = SwapWithRandomInBucket(fingerprint, index_bucket, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (InsertInBucket(fingerprint, index_bucket)) {
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

// Ivan & Tia
inline bool Segment::InsertInBucket(uint32_t fingerprint, uint32_t index_bucket) {
    if (buckets_[index_bucket].size() < kFingerprintsPerBucket) {
        buckets_[index_bucket].push_back(fingerprint);
        return true;
    }

    return false;
}

// Ivan
[[nodiscard]] inline uint32_t Segment::SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t entry_index = rng() % kFingerprintsPerBucket;
    uint32_t taken = buckets_[index_bucket][entry_index];
    buckets_[index_bucket][entry_index] = fingerprint;
    return taken;
}