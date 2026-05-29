#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

// Configurations
inline constexpr size_t kNumBitsBucket = 10;
inline constexpr size_t kFingerprintsPerBucket = 4; // Helps some operations if this is a power of 2, but it is not necessary
inline constexpr size_t kNumBitsFingerprint = 12;
inline constexpr size_t kMaxEvictions = 8;
inline constexpr size_t kResizingThreshold = 2 * 4 * (1 << kNumBitsBucket); // Must be a power of 2

// Compiler checks
static_assert(kNumBitsBucket < std::numeric_limits<uint32_t>::digits);
static_assert(kNumBitsFingerprint < std::numeric_limits<uint32_t>::digits);
static_assert(kNumBitsBucket + kNumBitsFingerprint < std::numeric_limits<uint32_t>::digits); // Leave Space for initial segment bits

using fingerprint_t = std::conditional_t<
    kNumBitsFingerprint <= 8,
    uint8_t,
    std::conditional_t<
        kNumBitsFingerprint <= 16,
        uint16_t,
        uint32_t
    >
>;

// Constants based on configurations
inline constexpr size_t kBucketsPerSegment = size_t{1} << kNumBitsBucket;
inline constexpr uint32_t kMaskBucket = (uint32_t{1} << kNumBitsBucket) - uint32_t{1};
inline constexpr uint32_t kMaskFingerprint = (uint32_t{1} << kNumBitsFingerprint) - uint32_t{1};

#endif