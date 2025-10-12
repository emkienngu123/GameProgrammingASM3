#pragma once
namespace Utils {
    template<class Enum>
    constexpr size_t EnumCast(Enum value) noexcept {
        return static_cast<size_t>(value);
    }

    template<class Enum>
    constexpr size_t EnumSize() noexcept {
        return EnumCast(Enum::COUNT);
    }

    template<class Enum>
    constexpr Enum EnumCast(size_t value) noexcept {
        return static_cast<Enum>(value);
    }

    // Recursive CreateMask for C++14
    constexpr size_t CreateMask() noexcept { return 0; }

    template<typename T, typename... Args>
    constexpr size_t CreateMask(T bit, Args... bits) noexcept {
        return static_cast<size_t>(bit) | CreateMask(bits...);
    }

    template<typename... Args>
    constexpr bool HasAny(size_t mask, Args... bits) noexcept {
        return (mask & CreateMask(bits...)) > 0;
    }
}
