#pragma once
#include "char_queue.hpp"
#include "mem.hpp"

#include <string>
#include <vector>

namespace mem {
    class region;

    class pattern {
    private:
        std::vector<byte> bytes_{};
        std::vector<byte> masks_{};
        std::size_t trimmed_size_{ 0 };
        bool needs_masks_{ true };

        void finalize();

        bool parse_chunk(char_queue& input, char wildcard);

    public:
        explicit pattern() = default;

        enum class wildcard_t : char
        {
        };

        explicit pattern(const char* string, wildcard_t wildcard = static_cast<wildcard_t>('?'));
        explicit pattern(const void* bytes, const char* masks, wildcard_t wildcard = static_cast<wildcard_t>('?'));

        explicit pattern(const void* bytes, const void* masks, std::size_t length);

        bool match(pointer address) const noexcept;

        const byte* bytes() const noexcept;
        const byte* masks() const noexcept;

        std::size_t size() const noexcept;
        std::size_t trimmed_size() const noexcept;

        bool needs_masks() const noexcept;

        std::size_t get_skip_pos(const byte* frequencies) const noexcept;

        explicit operator bool() const noexcept;

        std::string to_string() const;
    };

    mem::pointer scan(const mem::pattern& pattern, mem::region range);
    std::vector<mem::pointer> scan_all(const mem::pattern& pattern, mem::region range);

    inline bool pattern::parse_chunk(char_queue& input, char wildcard) {
        byte value = 0x00;
        byte mask = 0x00;

        std::size_t count = 1;

        int current = -1;
        int temp = -1;

        // clang-format off
        current = input.peek();
        if ((temp = xctoi(current)) != -1) { input.pop(); value = static_cast<byte>(temp); mask = 0xFF; }
        else if (current == wildcard) { input.pop(); value = 0x00;                    mask = 0x00; }
        else { return false; }

        current = input.peek();
        if ((temp = xctoi(current)) != -1) { input.pop(); value = (value << 4) | static_cast<byte>(temp); mask = (mask << 4) | 0x0F; }
        else if (current == wildcard) { input.pop(); value = (value << 4);                           mask = (mask << 4); }

        if (input.peek() == '&') {
            input.pop();

            byte expl_mask = 0xFF;

            if ((temp = xctoi(input.peek())) != -1) { input.pop(); expl_mask = static_cast<byte>(temp); }
            else { return false; }

            if ((temp = xctoi(input.peek())) != -1) { input.pop(); expl_mask = (expl_mask << 4) | static_cast<byte>(temp); }

            mask &= expl_mask;
        }

        if (input.peek() == '#') {
            input.pop();

            count = 0;

            while (true) {
                if ((temp = dctoi(input.peek())) != -1) { input.pop(); count = (count * 10) + static_cast<std::size_t>(temp); }
                else if (count > 0) { break; }
                else { return false; }
            }
        }
        // clang-format on

        value &= mask;

        for (std::size_t i = 0; i < count; ++i) {
            bytes_.push_back(value);
            masks_.push_back(mask);
        }

        return true;
    }

    inline pattern::pattern(const char* string, wildcard_t wildcard) {
        char_queue input(string);

        while (input) {
            if (input.peek() == ' ') {
                input.pop();

                continue;
            }

            if (!parse_chunk(input, static_cast<char>(wildcard))) {
                masks_.clear();
                bytes_.clear();

                break;
            }
        }

        finalize();
    }

    inline pattern::pattern(const void* bytes, const char* mask, wildcard_t wildcard) {
        if (mask) {
            const std::size_t size = std::strlen(mask);

            bytes_.resize(size);
            masks_.resize(size);

            for (std::size_t i = 0; i < size; ++i) {
                if (mask[i] == static_cast<char>(wildcard)) {
                    bytes_[i] = 0x00;
                    masks_[i] = 0x00;
                }
                else {
                    bytes_[i] = static_cast<const byte*>(bytes)[i];
                    masks_[i] = 0xFF;
                }
            }
        }
        else {
            const std::size_t size = std::strlen(static_cast<const char*>(bytes));

            bytes_.resize(size);
            masks_.resize(size);

            for (std::size_t i = 0; i < size; ++i) {
                bytes_[i] = static_cast<const byte*>(bytes)[i];
                masks_[i] = 0xFF;
            }
        }

        finalize();
    }

    inline pattern::pattern(const void* bytes, const void* mask, std::size_t length) {
        if (mask) {
            bytes_.resize(length);
            masks_.resize(length);

            for (std::size_t i = 0; i < length; ++i) {
                const byte v = static_cast<const byte*>(bytes)[i];
                const byte m = static_cast<const byte*>(mask)[i];

                bytes_[i] = v & m;
                masks_[i] = m;
            }
        }
        else {
            bytes_.resize(length);
            masks_.resize(length);

            for (std::size_t i = 0; i < length; ++i) {
                bytes_[i] = static_cast<const byte*>(bytes)[i];
                masks_[i] = 0xFF;
            }
        }

        finalize();
    }

    inline void pattern::finalize() {
        if (bytes_.size() != masks_.size()) {
            bytes_.clear();
            masks_.clear();
            trimmed_size_ = 0;
            needs_masks_ = false;

            return;
        }

        for (std::size_t i = 0; i < bytes_.size(); ++i)
            bytes_[i] &= masks_[i];

        std::size_t trimmed_size = bytes_.size();

        while (trimmed_size && (masks_[trimmed_size - 1] == 0x00))
            --trimmed_size;

        trimmed_size_ = trimmed_size;

        needs_masks_ = false;

        for (std::size_t i = trimmed_size_; i--;) {
            if (masks_[i] != 0xFF) {
                needs_masks_ = true;

                break;
            }
        }
    }

    inline bool pattern::match(pointer address) const noexcept {
        const byte* const pat_bytes = bytes();

        if (!pat_bytes)
            return false;

        const byte* current = address.as<const byte*>();

        const std::size_t last = trimmed_size() - 1;

        if (needs_masks()) {
            const byte* const pat_masks = masks();

            for (std::size_t i = last; mem_likely((current[i] & pat_masks[i]) != pat_bytes[i]); --i) {
                if (mem_unlikely(i == 0))
                    return true;
            }

            return false;
        }
        else {
            for (std::size_t i = last; mem_likely(current[i] != pat_bytes[i]); --i) {
                if (mem_unlikely(i == 0))
                    return true;
            }

            return false;
        }
    }

    strong_inline const byte* pattern::bytes() const noexcept {
        return !bytes_.empty() ? bytes_.data() : nullptr;
    }

    strong_inline const byte* pattern::masks() const noexcept {
        return !masks_.empty() ? masks_.data() : nullptr;
    }

    strong_inline std::size_t pattern::size() const noexcept {
        return bytes_.size();
    }

    strong_inline std::size_t pattern::trimmed_size() const noexcept {
        return trimmed_size_;
    }

    strong_inline bool pattern::needs_masks() const noexcept {
        return needs_masks_;
    }

    strong_inline std::size_t pattern::get_skip_pos(const byte* frequencies) const noexcept {
        std::size_t min = SIZE_MAX;
        std::size_t result = SIZE_MAX;

        for (std::size_t i = 0; i < size(); ++i) {
            if (masks_[i] == 0xFF) {
                std::size_t f = frequencies[bytes_[i]];

                if (f <= min) {
                    result = i;
                    min = f;
                }
            }
        }

        return result;
    }

    strong_inline pattern::operator bool() const noexcept {
        return !bytes_.empty() && !masks_.empty();
    }

    inline std::string pattern::to_string() const {
        const char* const hex_chars = "0123456789ABCDEF";

        std::string result;

        for (std::size_t i = 0; i < size(); ++i) {
            if (i)
                result += ' ';

            const byte mask = masks_[i];
            const byte value = bytes_[i];

            if (mask != 0x00) {
                result += hex_chars[static_cast<std::size_t>(value >> 4)];
                result += hex_chars[static_cast<std::size_t>(value & 0xF)];

                if (mask != 0xFF) {
                    result += '&';
                    result += hex_chars[static_cast<std::size_t>(mask >> 4)];
                    result += hex_chars[static_cast<std::size_t>(mask & 0xF)];
                }
            }
            else
                result += '?';
        }

        return result;
    }

    template <typename Scanner>
    class scanner_base {
    public:
        pointer operator()(region range) const;

        template <typename Func>
        pointer operator()(region range, Func func) const;

        std::vector<pointer> scan_all(region range) const;
    };

    template <typename Scanner>
    strong_inline pointer scanner_base<Scanner>::operator()(region range) const {
        return static_cast<const Scanner*>(this)->scan(range);
    }

    template <typename Scanner>
    template <typename Func>
    inline pointer scanner_base<Scanner>::operator()(region range, Func func) const {
        while (true) {
            const pointer result = static_cast<const Scanner*>(this)->scan(range);

            if (result) {
                if (func(result))
                    return result;

                range = range.sub_region(result + 1);
            }
            else
                break;
        }

        return nullptr;
    }

    template <typename Scanner>
    inline std::vector<pointer> scanner_base<Scanner>::scan_all(region range) const {
        std::vector<pointer> results;

        (*this)(range, [&results](pointer result) {
            results.emplace_back(result);

            return false;
            });

        return results;
    }
}

#include "simd_scanner.hpp"

namespace mem {
    using default_scanner = class simd_scanner;

    inline mem::pointer scan(const mem::pattern& pattern, mem::region range) {
        return mem::default_scanner(pattern).scan(range);
    }

    inline std::vector<mem::pointer> scan_all(const mem::pattern& pattern, mem::region range) {
        return mem::default_scanner(pattern).scan_all(range);
    }
}