#include "segment.h"

#include <cstdint>
#include <random>
#include <vector>
#include <algorithm>

#include "config.h"

// Ivan
Segment::Segment()
        : buckets_{},
        overflow_{nullptr} {}

// Tia
Segment::Segment(Segment* original)
        : buckets_{original->buckets_},
        overflow_{nullptr} {}

// Tia
Segment::~Segment() {
    delete overflow_;
}

// Tia
[[nodiscard]] const std::array<Bucket, kBucketsPerSegment>& Segment::GetBuckets() const noexcept {
    return buckets_;
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
        InsertResult insert_result = segment->InsertLocal(fingerprint, index_bucket, index_bucket_other, rng);
        if (insert_result.success) return true;

        fingerprint = insert_result.leftover;
        index_bucket = insert_result.bucket;
        index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

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


// Ivan
bool Segment::MergeSegment(Segment* other, std::mt19937& rng) {
    while (other != nullptr) {
        for (size_t bucket_index = 0 ; bucket_index < kBucketsPerSegment ; bucket_index++) {
            for (uint8_t i = 0 ; i < other->buckets_[bucket_index].GetSize() ; i++) {
                Insert(other->buckets_[bucket_index].GetEntries()[i], bucket_index, rng);
            }
        }

        other = other->GetOverflow();
    }

    return true;
}

// Ivan
void Segment::ClearOverflow() {
    delete overflow_;
    overflow_ = nullptr;
}

// Ivan & Tia
InsertResult Segment::InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937 &rng) {
    if (buckets_[index_bucket].Insert(fingerprint) || buckets_[index_bucket_other].Insert(fingerprint)) {
        return {true, 0, 0};
    }

    if (rng() & uint32_t{1}) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (size_t i = 0 ; i < kMaxEvictions ; i++) {
        fingerprint = buckets_[index_bucket].SwapWithRandom(fingerprint, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (buckets_[index_bucket].Insert(fingerprint)) {
            return {true, 0, 0};
        }
    }

    return {false, fingerprint, index_bucket};
}

// Ivan
void Segment::AddOverflow() {
    overflow_ = new Segment();
}

// Ivan
[[nodiscard]] inline uint32_t Segment::GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const {
    return (index_bucket ^ fingerprint) & kMaskBucket;
}