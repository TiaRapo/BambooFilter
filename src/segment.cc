#include "segment.h"

#include <random>

#include "utility.h"
#include "config.h"

Segment::Segment(/*TODO*/)
        : buckets_(kNumBitsBucket, std::vector<uint32_t>(kFingerprintsPerBucket, -1)) {
    // TODO ?
}

Segment::Segment(Segment* original) {
    // TODO
}

Segment::~Segment() {
    // TODO ?
}

[[nodiscard]] Segment* Segment::GetOverflow() noexcept {
    return overflow_;
}

// TODO:
// It might be costly time-wise to always send the RNG
// Rng is a big element so we don't want each segment to have it's own
// But putting it as static between segments is not good practice
// Maybe find a different way to pick a random entry
bool Segment::Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t index_bucket_other = GetOtherBucket(index_bucket, fingerprint);

    if (InsertInBucket(fingerprint, index_bucket) || InsertInBucket(fingerprint, index_bucket)) {
        return true;
    }

    if (rng() & 1u) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (std::size_t i = 0 ; i < kMaxEvictions_ ; i++) {
        fingerprint = SwapWithRandomInBucket(fingerprint, index_bucket, rng);
        index_bucket = GetOtherBucket(index_bucket, fingerprint);
        if (InsertInBucket(fingerprint, index_bucket)) {
            return true;
        }
    }

    return false;
}

bool Segment::Lookup(uint32_t fingerprint, uint32_t index_bucket) const {
    // TODO
}

bool Segment::Delete(uint32_t fingerprint, uint32_t index_bucket) {
    // TODO
}

bool Segment::EraseByBit(bool bit_value, std::uint32_t bit_index) {
    // TODO
}

[[nodiscard]] inline uint32_t GetOtherBucket(uint32_t index_bucket, uint32_t fingerprint) {
    return (index_bucket ^ fingerprint) & kMaskBucket;
}

inline bool Segment::InsertInBucket(uint32_t fingerprint, uint32_t index_bucket) {
    for (auto &entry : buckets_[index_bucket]) {
        if (entry == -1) {
            entry = fingerprint;
            return true;
        }
    }
}

[[nodiscard]] inline uint32_t Segment::SwapWithRandomInBucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t entry_index = rng() % kFingerprintsPerBucket;
    uint32_t taken = buckets_[index_bucket][entry_index];
    buckets_[index_bucket][entry_index] = fingerprint;
    return taken;
}