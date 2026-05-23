#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>
#include <cstdint>

// Configurations / Global constants

inline constexpr std::size_t kNumBitsBucket = 10;
inline constexpr std::size_t kBucketsPerSegment = (std::size_t{1} << kNumBitsBucket);
inline constexpr std::uint32_t kMaskBucket = (std::uint32_t{1} << kNumBitsBucket) - std::uint32_t{1};

inline constexpr std::size_t kFingerprintsPerBucket = 4;

inline constexpr std::size_t kNumBitsFingerprint = 12;
inline constexpr std::uint32_t kMaskFingerprint = (std::uint32_t{1} << kNumBitsFingerprint) - std::uint32_t{1};
inline constexpr std::uint32_t kEmptyFingerprint = 0u;
inline constexpr std::uint32_t kEmptyFingerprintReplacement = 1u;

inline constexpr std::size_t kResizingThreshold = 8; // Must be a power of 2 // TODO: Definitely adjust this

inline constexpr std::size_t kMaxEvictions = 8;

#endif