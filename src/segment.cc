#include "segment.h"

#include <random>

#include "utility.h"
#include "config.h"

Segment::Segment(/*TODO?*/)
        : buckets_(2 << kNumBitsBucket, std::vector<uint32_t>(kFingerprintsPerBucket, kEmptyFingerprint)),
        overflow_(nullptr) {
    // TODO ?
}

Segment::Segment(Segment* original) {
    // TODO
}

Segment::~Segment() {
    // TODO ?
}

[[nodiscard]] Segment* Segment::GetOverflow() const noexcept {
    return overflow_;
}

// TODO:
// It might be costly time-wise to always send the RNG
// Rng is a big element so we don't want each segment to have it's own
// But putting it as static between segments is not good practice
// Maybe find a different way to pick a random entry
bool Segment::Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937& rng) {
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    Segment* segment = this;
    
    while (true) {
        if (segment->InsertLocal(fingerprint, index_bucket, index_bucket_other, rng)) {
            break;
        }

        Segment* overflow = segment->GetOverflow();
        if (overflow == nullptr) {
            segment->AddOverflow();
            segment = segment->GetOverflow();
            segment->InsertLocal(fingerprint, index_bucket, index_bucket_other, rng);
            break;
        }

        segment = overflow;
    }
    
    if (InsertInBucket(fingerprint, index_bucket) || InsertInBucket(fingerprint, index_bucket_other)) {
        return true;
    }

    if (rng() & std::uint32_t{1}) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (std::size_t i = 0 ; i < kMaxEvictions ; i++) {
        fingerprint = SwapWithRandomInBucket(fingerprint, index_bucket, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (InsertInBucket(fingerprint, index_bucket)) {
            return true;
        }
    }

    return false;
}

bool Segment::Insert(Segment& other, std::mt19937& rng) {
    for (std::size_t i = 0 ; i < kBucketsPerSegment ; i++) {
        for (std::size_t j = 0 ; j < kFingerprintsPerBucket ; j++) {
            Insert(other.buckets_[i][j], i, rng);
        }
    }

    return true;
}

bool Segment::Lookup(uint32_t fingerprint, uint32_t index_bucket) const {
    for (auto &entry : buckets_[index_bucket]) {
        if (entry == fingerprint) {
            return true;
        }
    }
    
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    for (auto &entry : buckets_[index_bucket_other]) {
        if (entry == fingerprint) {
            return true;
        }
    }

    return false;
}

bool Segment::Delete(uint32_t fingerprint, uint32_t index_bucket) {
    for (auto &entry : buckets_[index_bucket]) {
        if (entry == fingerprint) {
            entry = kEmptyFingerprint;
            return true;
        }
    }
    
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    for (auto &entry : buckets_[index_bucket_other]) {
        if (entry == fingerprint) {
            entry = kEmptyFingerprint;
            return true;
        }
    }

    return false;
}

void Segment::EraseByBit(bool bit_value, std::uint32_t bit_index) {
    for (auto &bucket : buckets_) {
        for (auto &entry : bucket) {
            bool bit_is_set = (entry & (std::uint32_t{1} << bit_index)) != std::uint32_t{0};
            
            if (bit_is_set == bit_value) {
                entry = kEmptyFingerprint;
            }
        }
    }
}

bool Segment::InsertLocal(uint32_t fingerprint, uint32_t index_bucket, uint32_t index_bucket_other, std::mt19937 &rng) {
    if (InsertInBucket(fingerprint, index_bucket) || InsertInBucket(fingerprint, index_bucket_other)) {
        return true;
    }

    if (rng() & std::uint32_t{1}) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (std::size_t i = 0 ; i < kMaxEvictions ; i++) {
        fingerprint = SwapWithRandomInBucket(fingerprint, index_bucket, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (InsertInBucket(fingerprint, index_bucket)) {
            return true;
        }
    }

    return false;
}

void Segment::AddOverflow() {
    overflow_ = new Segment();
}

[[nodiscard]] inline uint32_t Segment::GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) const {
    return (index_bucket ^ fingerprint) & kMaskBucket;
}

inline bool Segment::InsertInBucket(uint32_t fingerprint, uint32_t index_bucket) {
    for (auto &entry : buckets_[index_bucket]) {
        if (entry == kEmptyFingerprint) {
            entry = fingerprint;
            return true;
        }
    }

    return false;
}

[[nodiscard]] inline uint32_t Segment::SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t entry_index = rng() % kFingerprintsPerBucket;
    uint32_t taken = buckets_[index_bucket][entry_index];
    buckets_[index_bucket][entry_index] = fingerprint;
    return taken;
}