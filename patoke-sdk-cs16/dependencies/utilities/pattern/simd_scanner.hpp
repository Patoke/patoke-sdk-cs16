#pragma once
#include "pattern.hpp"

#define MEM_SIMD_SCANNER_USE_MEMCHR

namespace mem {
    class simd_scanner : public scanner_base<simd_scanner> {
    private:
        const pattern* pattern_{ nullptr };
        std::size_t skip_pos_{ SIZE_MAX };

    public:
        simd_scanner() = default;

        simd_scanner(const pattern& pattern);
        simd_scanner(const pattern& pattern, const byte* frequencies);

        pointer scan(region range) const;

        static const byte* default_frequencies() noexcept;
    };

    const byte* find_byte(const byte* ptr, byte value, std::size_t num);

    inline simd_scanner::simd_scanner(const pattern& _pattern)
        : simd_scanner(_pattern, default_frequencies())
    {}

    inline simd_scanner::simd_scanner(const pattern& _pattern, const byte* frequencies)
        : pattern_(&_pattern)
        , skip_pos_(_pattern.get_skip_pos(frequencies))
    {}

    strong_inline const byte* simd_scanner::default_frequencies() noexcept {
        static constexpr const byte frequencies[256] {
            0xFF,0xFB,0xF2,0xEE,0xEC,0xE7,0xDC,0xC8,0xED,0xB7,0xCC,0xC0,0xD3,0xCD,0x89,0xFA,
            0xF3,0xD6,0x8D,0x83,0xC1,0xAA,0x7A,0x72,0xC6,0x60,0x3E,0x2E,0x98,0x69,0x39,0x7C,
            0xEB,0x76,0x24,0x34,0xF9,0x50,0x04,0x07,0xE5,0xAC,0x53,0x65,0x9B,0x4D,0x6D,0x5C,
            0xDA,0x93,0x7F,0xCB,0x92,0x49,0x43,0x09,0xBA,0x8E,0x1E,0x91,0x8A,0x5B,0x11,0xA1,
            0xE8,0xF5,0x9E,0xAD,0xEF,0xE6,0x79,0x7B,0xFE,0xE0,0x1F,0x54,0xE4,0xBD,0x7D,0x6A,
            0xDF,0x67,0x7E,0xA4,0xB6,0xAF,0x88,0xA0,0xC3,0xA9,0x26,0x77,0xD1,0x71,0x61,0xC2,
            0x9A,0xCA,0x29,0x9F,0xD8,0xE2,0xD0,0x6E,0xB4,0xB8,0x25,0x3C,0xBF,0x73,0xB5,0xCF,
            0xD4,0x01,0xCE,0xBE,0xF1,0xDB,0x52,0x37,0x9D,0x63,0x02,0x6B,0x80,0x45,0x2B,0x95,
            0xE1,0xC4,0x36,0xF0,0xD5,0xE3,0x57,0x9C,0xB1,0xF7,0x82,0xFC,0x42,0xF6,0x18,0x33,
            0xD2,0x48,0x05,0x0F,0x41,0x1D,0x03,0x27,0x70,0x10,0x00,0x08,0x55,0x16,0x2F,0x0E,
            0x94,0x35,0x2C,0x40,0x6F,0x3B,0x1C,0x28,0x90,0x68,0x81,0x4B,0x56,0x30,0x2A,0x3D,
            0x97,0x17,0x06,0x13,0x32,0x0B,0x5A,0x75,0xA5,0x86,0x78,0x4F,0x2D,0x51,0x46,0x5F,
            0xE9,0xDE,0xA2,0xDD,0xC9,0x4C,0xAB,0xBB,0xC7,0xB9,0x74,0x8F,0xF8,0x6C,0x85,0x8B,
            0xC5,0x84,0x8C,0x66,0x21,0x23,0x64,0x59,0xA3,0x87,0x44,0x58,0x3A,0x0D,0x12,0x19,
            0xAE,0x5E,0x3F,0x38,0x31,0x22,0x0A,0x14,0xF4,0xD9,0x20,0xB0,0xB2,0x1A,0x0C,0x15,
            0xB3,0x47,0x5D,0xEA,0x4A,0x1B,0x99,0xBC,0xD7,0xA6,0x62,0x4E,0xA8,0x96,0xA7,0xFD,
        };

        return frequencies;
    }

    inline pointer simd_scanner::scan(region range) const {
        const std::size_t trimmed_size = pattern_->trimmed_size();

        if (!trimmed_size)
            return nullptr;

        const std::size_t original_size = pattern_->size();
        const std::size_t region_size = range.size;

        if (original_size > region_size)
            return nullptr;

        const byte* const region_base = range.start.as<const byte*>();
        const byte* const region_end = region_base + region_size;

        const byte* current = region_base;
        const byte* const end = region_end - original_size + 1;

        const std::size_t last = trimmed_size - 1;

        const byte* const pat_bytes = pattern_->bytes();

        const std::size_t skip_pos = skip_pos_;

        if (skip_pos != SIZE_MAX) {
            if (pattern_->needs_masks()) {
                const byte* const pat_masks = pattern_->masks();

                while (mem_likely(current < end)) {
                    [[likely]];

                    for (std::size_t i = last; mem_likely((current[i] & pat_masks[i]) == pat_bytes[i]); --i) {
                        [[likely]];

                        if (mem_unlikely(i == 0)) [[unlikely]]
                            return current;
                    }

                    ++current;
                    current =
                        find_byte(current + skip_pos, pat_bytes[skip_pos], static_cast<std::size_t>(end - current)) -
                        skip_pos;
                }

                return nullptr;
            }
            else {
                while (mem_likely(current < end)) {
                    [[likely]];

                    for (std::size_t i = last; mem_likely(current[i] == pat_bytes[i]); --i) {
                        [[likely]];

                        if (mem_unlikely(i == 0)) [[unlikely]]
                            return current;
                    }

                    ++current;
                    current =
                        find_byte(current + skip_pos, pat_bytes[skip_pos], static_cast<std::size_t>(end - current)) -
                        skip_pos;
                }

                return nullptr;
            }
        }
        else {
            const byte* const pat_masks = pattern_->masks();

            while (mem_likely(current < end)) {
                [[likely]];

                for (std::size_t i = last; mem_likely((current[i] & pat_masks[i]) == pat_bytes[i]); --i) {
                    [[likely]];

                    if (mem_unlikely(i == 0)) [[unlikely]]
                        return current;
                }

                ++current;
            }

            return nullptr;
        }
    }

    strong_inline const byte* find_byte(const byte* ptr, byte value, std::size_t num) {
        const byte* result = static_cast<const byte*>(std::memchr(ptr, value, num));

        if (mem_unlikely(result == nullptr)) [[unlikely]]
            result = ptr + num;

        return result;
    }
}