#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <rpcndr.h>

namespace mem {
    class any_pointer;

    class pointer
    {
    private:
        std::uintptr_t value_{ 0 };

    public:
        constexpr pointer() noexcept;

        constexpr pointer(std::nullptr_t) noexcept;
        constexpr pointer(std::uintptr_t address) noexcept;

        template <typename T>
        pointer(T* address) noexcept;

        template <typename T, typename C>
        pointer(T C::* address) noexcept;

        constexpr pointer add(std::size_t count) const noexcept;
        constexpr pointer sub(std::size_t count) const noexcept;

        constexpr pointer offset(std::ptrdiff_t count) const noexcept;

        constexpr pointer shift(pointer from, pointer to) const noexcept;

        constexpr pointer align_up(std::size_t align) const noexcept;
        constexpr pointer align_down(std::size_t align) const noexcept;

#if defined(MEM_ARCH_X86_64)
        pointer rip(std::size_t offset) const noexcept;
#endif // MEM_ARCH_X86_64

        pointer& deref() const noexcept;

        constexpr pointer operator+(std::size_t count) const noexcept;
        constexpr pointer operator-(std::size_t count) const noexcept;

        constexpr std::ptrdiff_t operator-(pointer rhs) const noexcept;

        constexpr pointer& operator+=(std::size_t count) noexcept;
        constexpr pointer& operator-=(std::size_t count) noexcept;

        constexpr pointer& operator++() noexcept;
        constexpr pointer& operator--() noexcept;

        constexpr pointer operator++(int) noexcept;
        constexpr pointer operator--(int) noexcept;

        constexpr bool operator==(pointer rhs) const noexcept;
        constexpr bool operator!=(pointer rhs) const noexcept;

        constexpr bool operator<(pointer rhs) const noexcept;
        constexpr bool operator>(pointer rhs) const noexcept;

        constexpr bool operator<=(pointer rhs) const noexcept;
        constexpr bool operator>=(pointer rhs) const noexcept;

        constexpr bool operator!() const noexcept;

        constexpr explicit operator bool() const noexcept;

        template <typename T = pointer>
        typename std::add_lvalue_reference<T>::type at(std::size_t offset) const noexcept;

        template <typename T>
        constexpr typename std::enable_if<std::is_integral<T>::value, T>::type as() const noexcept;

        template <typename T>
        typename std::enable_if<std::is_pointer<T>::value, T>::type as() const noexcept;

        template <typename T>
        typename std::enable_if<std::is_member_pointer<T>::value, T>::type as() const noexcept;

        template <typename T>
        typename std::enable_if<std::is_lvalue_reference<T>::value, T>::type as() const noexcept;

        template <typename T>
        typename std::enable_if<std::is_array<T>::value, typename std::add_lvalue_reference<T>::type>::type
            as() const noexcept;

        template <typename T>
        typename std::enable_if<!std::is_reference<T>::value, typename std::add_lvalue_reference<T>::type>::type
            rcast() & noexcept;

        template <typename Func>
        constexpr pointer and_then(Func&& func) const;

        template <typename Func>
        constexpr pointer or_else(Func&& func) const;

        constexpr any_pointer any() const noexcept;
    };

    static_assert((sizeof(pointer) == sizeof(void*)) && (alignof(pointer) == alignof(void*)), "Hmm...");

    class any_pointer
    {
    private:
        std::uintptr_t value_{ 0 };

    public:
        constexpr any_pointer(std::uintptr_t value) noexcept;

        constexpr operator std::uintptr_t() const noexcept;

        template <typename T>
        operator T* () const noexcept;
    };

    class region
    {
    public:
        pointer start{ nullptr };
        std::size_t size{ 0 };

        constexpr region() noexcept;

        constexpr region(pointer start, std::size_t size) noexcept;

        constexpr bool contains(region rhs) const noexcept;

        constexpr bool contains(pointer address) const noexcept;
        constexpr bool contains(pointer start, std::size_t size) const noexcept;

        template <typename T>
        constexpr bool contains(pointer address) const noexcept;

        constexpr bool operator==(region rhs) const noexcept;
        constexpr bool operator!=(region rhs) const noexcept;

        void copy(pointer source) const noexcept;
        void fill(byte value) const noexcept;

        constexpr region sub_region(pointer address) const noexcept;
    };

    template <typename T>
    typename std::add_lvalue_reference<T>::type field(pointer base, std::ptrdiff_t offset = 0) noexcept;

    template <typename F>
    typename std::add_lvalue_reference<F>::type vfunc(
        pointer inst, std::size_t index, std::ptrdiff_t table = 0) noexcept;

    template <typename To, typename From>
    To bit_cast(const From& src) noexcept;

    strong_inline constexpr pointer::pointer() noexcept = default;

    strong_inline constexpr pointer::pointer(std::nullptr_t) noexcept
        : value_(0)
    {}

    strong_inline constexpr pointer::pointer(std::uintptr_t address) noexcept
        : value_(address)
    {}

    template <typename T>
    strong_inline pointer::pointer(T* address) noexcept
        : value_(reinterpret_cast<std::uintptr_t>(address))
    {}

    template <typename T, typename C>
    strong_inline pointer::pointer(T C::* address) noexcept
        : value_(bit_cast<std::uintptr_t>(address))
    {}

    strong_inline constexpr pointer pointer::add(std::size_t count) const noexcept
    {
        return value_ + count;
    }

    strong_inline constexpr pointer pointer::sub(std::size_t count) const noexcept
    {
        return value_ - count;
    }

    strong_inline constexpr pointer pointer::offset(std::ptrdiff_t count) const noexcept
    {
        return static_cast<std::uintptr_t>(static_cast<std::intptr_t>(value_) + count);
    }

    strong_inline constexpr pointer pointer::shift(pointer from, pointer to) const noexcept
    {
        return (value_ - from.value_) + to.value_;
    }

    strong_inline constexpr pointer pointer::align_up(std::size_t align) const noexcept
    {
        return (value_ + align - 1) / align * align;
    }

    strong_inline constexpr pointer pointer::align_down(std::size_t align) const noexcept
    {
        return value_ - (value_ % align);
    }

#if defined(MEM_ARCH_X86_64)
    strong_inline pointer pointer::rip(std::size_t offset) const noexcept
    {
        return static_cast<std::uintptr_t>(
            static_cast<std::intptr_t>(value_ + offset) + *reinterpret_cast<const std::int32_t*>(value_));
    }
#endif // MEM_ARCH_X86_64

    strong_inline pointer& pointer::deref() const noexcept
    {
        return *reinterpret_cast<pointer*>(value_);
    }

    strong_inline constexpr pointer pointer::operator+(std::size_t count) const noexcept
    {
        return value_ + count;
    }

    strong_inline constexpr pointer pointer::operator-(std::size_t count) const noexcept
    {
        return value_ - count;
    }

    strong_inline constexpr std::ptrdiff_t pointer::operator-(pointer rhs) const noexcept
    {
        return static_cast<std::ptrdiff_t>(static_cast<std::intptr_t>(value_) - static_cast<std::intptr_t>(rhs.value_));
    }

    strong_inline constexpr pointer& pointer::operator+=(std::size_t count) noexcept
    {
        value_ += count;

        return *this;
    }

    strong_inline constexpr pointer& pointer::operator-=(std::size_t count) noexcept
    {
        value_ -= count;

        return *this;
    }

    strong_inline constexpr pointer& pointer::operator++() noexcept
    {
        ++value_;

        return *this;
    }

    strong_inline constexpr pointer& pointer::operator--() noexcept
    {
        --value_;

        return *this;
    }

    strong_inline constexpr pointer pointer::operator++(int) noexcept
    {
        pointer result = *this;

        ++value_;

        return result;
    }

    strong_inline constexpr pointer pointer::operator--(int) noexcept
    {
        pointer result = *this;

        --value_;

        return result;
    }

    strong_inline constexpr bool pointer::operator==(pointer rhs) const noexcept
    {
        return value_ == rhs.value_;
    }

    strong_inline constexpr bool pointer::operator!=(pointer rhs) const noexcept
    {
        return value_ != rhs.value_;
    }

    strong_inline constexpr bool pointer::operator<(pointer rhs) const noexcept
    {
        return value_ < rhs.value_;
    }

    strong_inline constexpr bool pointer::operator>(pointer rhs) const noexcept
    {
        return value_ > rhs.value_;
    }

    strong_inline constexpr bool pointer::operator<=(pointer rhs) const noexcept
    {
        return value_ <= rhs.value_;
    }

    strong_inline constexpr bool pointer::operator>=(pointer rhs) const noexcept
    {
        return value_ >= rhs.value_;
    }

    strong_inline constexpr bool pointer::operator!() const noexcept
    {
        return !value_;
    }

    strong_inline constexpr pointer::operator bool() const noexcept
    {
        return value_ != 0;
    }

    template <typename T>
    strong_inline constexpr typename std::enable_if<std::is_integral<T>::value, T>::type
        pointer::as() const noexcept
    {
        static_assert(
            std::is_same<typename std::make_unsigned<T>::type, std::uintptr_t>::value, "Invalid Integer Type");

        return static_cast<T>(value_);
    }

    template <typename T>
    strong_inline typename std::add_lvalue_reference<T>::type pointer::at(std::size_t offset) const noexcept
    {
        return *reinterpret_cast<typename std::add_pointer<T>::type>(value_ + offset);
    }

    template <typename T>
    strong_inline typename std::enable_if<std::is_pointer<T>::value, T>::type pointer::as() const noexcept
    {
        return reinterpret_cast<T>(value_);
    }

    template <typename T>
    strong_inline typename std::enable_if<std::is_member_pointer<T>::value, T>::type pointer::as() const noexcept
    {
        return bit_cast<T>(value_);
    }

    template <typename T>
    strong_inline typename std::enable_if<std::is_lvalue_reference<T>::value, T>::type pointer::as() const noexcept
    {
        return *reinterpret_cast<typename std::add_pointer<T>::type>(value_);
    }

    template <typename T>
    strong_inline
        typename std::enable_if<std::is_array<T>::value, typename std::add_lvalue_reference<T>::type>::type
        pointer::as() const noexcept
    {
        return *reinterpret_cast<typename std::add_pointer<T>::type>(value_);
    }

    template <typename T>
    strong_inline
        typename std::enable_if<!std::is_reference<T>::value, typename std::add_lvalue_reference<T>::type>::type
        pointer::rcast() & noexcept
    {
        static_assert(sizeof(T) == sizeof(pointer), "That's no pointer. It's a space station.");

        return *reinterpret_cast<typename std::add_pointer<T>::type>(this);
    }

    template <typename Func>
    strong_inline constexpr pointer pointer::and_then(Func&& func) const
    {
        return value_ ? std::forward<Func>(func)(*this) : nullptr;
    }

    template <typename Func>
    strong_inline constexpr pointer pointer::or_else(Func&& func) const
    {
        return value_ ? *this : std::forward<Func>(func)();
    }

    strong_inline constexpr any_pointer pointer::any() const noexcept
    {
        return any_pointer(value_);
    }

    strong_inline constexpr any_pointer::any_pointer(std::uintptr_t value) noexcept
        : value_(value)
    {}

    strong_inline constexpr any_pointer::operator std::uintptr_t() const noexcept
    {
        return value_;
    }

    template <typename T>
    strong_inline any_pointer::operator T* () const noexcept
    {
        return reinterpret_cast<T*>(value_);
    }

    strong_inline constexpr region::region() noexcept = default;

    strong_inline constexpr region::region(pointer start_, std::size_t size_) noexcept
        : start(start_)
        , size(size_)
    {}

    strong_inline constexpr bool region::contains(region rhs) const noexcept
    {
        return (rhs.start >= start) && ((rhs.start + rhs.size) <= (start + size));
    }

    strong_inline constexpr bool region::contains(pointer address) const noexcept
    {
        return (address >= start) && (address < (start + size));
    }

    strong_inline constexpr bool region::contains(pointer start_, std::size_t size_) const noexcept
    {
        return (start_ >= start) && ((start_ + size_) <= (start + size));
    }

    template <typename T>
    strong_inline constexpr bool region::contains(pointer address) const noexcept
    {
        return (address >= start) && ((address + sizeof(T)) <= (start + size));
    }

    strong_inline constexpr bool region::operator==(region rhs) const noexcept
    {
        return (start == rhs.start) && (size == rhs.size);
    }

    strong_inline constexpr bool region::operator!=(region rhs) const noexcept
    {
        return (start != rhs.start) || (size != rhs.size);
    }

    strong_inline void region::copy(pointer source) const noexcept
    {
        std::memcpy(start.as<void*>(), source.as<const void*>(), size);
    }

    strong_inline void region::fill(byte value) const noexcept
    {
        std::memset(start.as<void*>(), value, size);
    }

    strong_inline constexpr region region::sub_region(pointer address) const noexcept
    {
        return region(address, size - static_cast<std::size_t>(address - start));
    }

    template <typename T>
    strong_inline typename std::add_lvalue_reference<T>::type field(pointer base, std::ptrdiff_t offset) noexcept
    {
        return base.at<T>(offset);
    }

    template <typename F>
    strong_inline typename std::add_lvalue_reference<F>::type vfunc(
        pointer inst, std::size_t index, std::ptrdiff_t table) noexcept
    {
        return inst.as<pointer**>()[table][index].rcast<F>();
    }

    template <typename To, typename From>
    strong_inline To bit_cast(const From& src) noexcept
    {
        static_assert(sizeof(To) == sizeof(From), "sizeof(To) != sizeof(From)");
        static_assert(std::is_trivially_copyable<From>::value, "From is not trivially copyable");
        static_assert(std::is_trivially_copyable<To>::value, "To is not trivially copyable");

        typename std::aligned_storage<sizeof(To), alignof(To)>::type dst;
        std::memcpy(&dst, &src, sizeof(To));
        return reinterpret_cast<To&>(dst);
    }
}