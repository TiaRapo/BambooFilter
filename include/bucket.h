#ifndef BUCKET_H
#define BUCKET_H

#include <random>
#include <array>
#include <cstdint>

#include "config.h"

class Bucket {
    public:
        // Constructors & Destructors
        Bucket();
        ~Bucket() = default;

        // Getters
        [[nodiscard]] uint8_t GetSize() const noexcept;
        [[nodiscard]] const std::array<fingerprint_t, kFingerprintsPerBucket>& GetEntries() const noexcept;

        // Operations
        bool Insert(fingerprint_t fingerprint);
        bool Delete(fingerprint_t fingerprint);
        bool Lookup(fingerprint_t fingerprint) const;
        void EraseByBit(bool bit_value, uint32_t bit_index);
        [[nodiscard]] fingerprint_t SwapWithRandom(fingerprint_t fingerprint, std::mt19937 &rng);

    private:
        // Attributes
        uint8_t size_;
        std::array<fingerprint_t, kFingerprintsPerBucket> entries_;
};

#endif