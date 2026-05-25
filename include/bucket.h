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
        [[nodiscard]] const std::array<uint32_t, kFingerprintsPerBucket>& GetEntries() const noexcept;

        // Operations
        bool Insert(uint32_t fingerprint);
        bool Delete(uint32_t fingerprint);
        bool Lookup(uint32_t fingerprint) const;
        void EraseByBit(bool bit_value, uint32_t bit_index);
        [[nodiscard]] uint32_t SwapWithRandom(uint32_t fingerprint, std::mt19937 &rng);

    private:
        // Attributes
        uint8_t size_;
        std::array<uint32_t, kFingerprintsPerBucket> entries_;
};