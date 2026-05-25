#include "bucket.h"

#include <random>
#include <array>
#include <cstdint>

#include "config.h"

// Ivan
Bucket::Bucket()
        : size_{0},
        entries_{} {}

// Ivan
[[nodiscard]] uint8_t Bucket::GetSize() const noexcept {
    return size_;
}

// Tia
[[nodiscard]] const std::array<uint32_t, kFingerprintsPerBucket>& Bucket::GetEntries() const noexcept {
    return entries_;
}

// Ivan
bool Bucket::Insert(uint32_t fingerprint) {
    if (size_ < kFingerprintsPerBucket) {
        entries_[size_] = fingerprint;
        size_++;
        return true;
    }

    return false;
}

// Tia
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

// Tia
bool Bucket::Lookup(uint32_t fingerprint) const {
    for (uint8_t i = 0 ; i < size_ ; i++) {
        if (entries_[i] == fingerprint) {
            return true;
        }
    }

    return false;
}

// Ivan & Tia
void Bucket::EraseByBit(bool bit_value, uint32_t bit_index) {
    for (uint8_t i = 0 ; i < size_ ; i++) {
        if (((entries_[i] & (uint32_t{1} << bit_index)) != uint32_t{0}) == bit_value) {
            entries_[i] = entries_[size_ - 1];
            size_--;
            i--;
        }
    }
}

// Ivan
[[nodiscard]] uint32_t Bucket::SwapWithRandom(uint32_t fingerprint, std::mt19937 &rng) {
    uint32_t entry_index = rng() % kFingerprintsPerBucket;
    uint32_t taken = entries_[entry_index];
    entries_[entry_index] = fingerprint;
    return taken;
}