#include "segment.h"

#include <random>

#include "utility.h"
#include "config.h"

Segment::Segment(/*TODO*/) {
    // TODO
}

Segment::~Segment() {
    // TODO ?
}

// TODO:
// It might be costly time-wise to always send the RNG
// Rng is a big element so we don't want each segment to have it's own
// But putting it as static between segments is not good practice
// Maybe find a different way to pick a random entry
bool Segment::Insert(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t index_bucket_other = get_other_bucket(index_bucket, fingerprint);

    if (insert_in_bucket(fingerprint, index_bucket) || insert_in_bucket(fingerprint, index_bucket)) {
        return true;
    }

    if (rng() & 1u) { // 50% chance to pick either of the two buckets
        index_bucket = index_bucket_other;
    }

    for (std::size_t i = 0 ; i < kMaxEvictions_ ; i++) {
        fingerprint = swap_with_random_in_bucket(fingerprint, index_bucket, rng);
        index_bucket = get_other_bucket(index_bucket, fingerprint);
        if (insert_in_bucket(fingerprint, index_bucket)) {
            return true;
        }
    }

    return false;
}

bool Segment::Lookup() const {
    // TODO
}

bool Segment::Delete() {
    // TODO
}

[[nodiscard]] inline uint32_t get_other_bucket(uint32_t index_bucket, uint32_t fingerprint) {
    return (index_bucket ^ fingerprint) & kMaskBucket;
}

inline bool Segment::insert_in_bucket(uint32_t fingerprint, uint32_t index_bucket) {
    for (auto &entry : buckets_[index_bucket]) {
        if (entry == 0) { // TODO: 0 does not mean empty probably
            entry = fingerprint;
            return true;
        }
    }
}

inline uint32_t Segment::swap_with_random_in_bucket(uint32_t fingerprint, uint32_t index_bucket, std::mt19937 &rng) {
    uint32_t entry_index = rng(); // TODO: Not like this
    uint32_t taken = buckets_[index_bucket][entry_index];
    buckets_[index_bucket][entry_index] = fingerprint;
    return taken;
}