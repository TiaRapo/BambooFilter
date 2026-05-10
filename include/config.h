#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>
#include <cstdint>

// Configurations / Global constants
inline constexpr std::size_t kNumBitsBucket = 10; // Buckets per segment
inline constexpr std::uint32_t kMaskBucket = (std::uint32_t{1} << kNumBitsBucket) - 1;

inline constexpr std::size_t kNumBitsFingerprint = 12;
inline constexpr std::uint32_t kMaskFingerprint = (std::uint32_t{1} << kNumBitsFingerprint) - 1;

#endif