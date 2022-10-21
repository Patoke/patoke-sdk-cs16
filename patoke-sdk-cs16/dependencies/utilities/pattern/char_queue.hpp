#pragma once
#include <cstring>

namespace mem {
    class char_queue {
    private:
        const char* start{ nullptr };
        const char* end{ nullptr };
        const char* current{ nullptr };

    public:
        char_queue(const char* string);
        constexpr char_queue(const char* string, std::size_t length);

        constexpr int peek() const noexcept;

        constexpr void pop() noexcept;
        constexpr std::size_t pos() const noexcept;

        constexpr explicit operator bool() const noexcept;
    };

    constexpr int xctoi(int value) noexcept;
    constexpr int dctoi(int value) noexcept;
    constexpr int octoi(int value) noexcept;

    strong_inline char_queue::char_queue(const char* string)
        : char_queue(string, std::strlen(string))
    {}

    strong_inline constexpr char_queue::char_queue(const char* string, std::size_t length)
        : start(string)
        , end(start + length)
        , current(start)
    {}

    strong_inline constexpr int char_queue::peek() const noexcept {
        return (current < end) ? static_cast<byte>(*current) : -1;
    }

    strong_inline constexpr void char_queue::pop() noexcept {
        if (current < end)
            ++current;
    }

    strong_inline constexpr std::size_t char_queue::pos() const noexcept {
        return static_cast<std::size_t>(current - start);
    }

    strong_inline constexpr char_queue::operator bool() const noexcept {
        return current < end;
    }

    strong_inline constexpr int xctoi(int value) noexcept {
        return (value >= '0' && value <= '9') ? (value - '0')
            : (value >= 'a' && value <= 'f') ? (value - 'a' + 10)
            : (value >= 'A' && value <= 'F') ? (value - 'A' + 10)
            : -1;
    }

    strong_inline constexpr int dctoi(int value) noexcept {
        return (value >= '0' && value <= '9') ? (value - '0') : -1;
    }

    strong_inline constexpr int octoi(int value) noexcept {
        return (value >= '0' && value <= '7') ? (value - '0') : -1;
    }
}