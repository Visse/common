#pragma once

#include <type_traits>

#define DECLARE_ENUM_FLAG( enum )                                   \
    inline constexpr enum operator | ( enum e1, enum e2 ) {         \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) |                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline constexpr enum operator & ( enum e1, enum e2 ) {         \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) &                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline constexpr enum operator ^ ( enum e1, enum e2 ) {         \
        return static_cast<enum>(                                   \
            std::underlying_type<enum>::type(e1) ^                  \
            std::underlying_type<enum>::type(e2)                    \
        );                                                          \
    }                                                               \
    inline constexpr enum operator ~ ( enum e1 ) {                  \
        return static_cast<enum>(                                   \
            ~std::underlying_type<enum>::type(e1)                   \
        );                                                          \
    }                                                               \
    inline constexpr bool any( enum e, enum flags ) {               \
        return std::underlying_type<enum>::type(e&flags) != 0;      \
    }                                                               \
    inline constexpr bool all( enum e, enum flags ) {               \
        return (e&flags) == flags;                                  \
    }                                                               \
    inline constexpr bool none( enum e, enum flags ) {              \
        return std::underlying_type<enum>::type(e&flags) == 0;      \
    }                                                               \
    inline constexpr enum clear( enum e, enum flags ) {             \
        return e & (~flags);                                        \
    }                                                               \
    inline constexpr enum set( enum e, enum flags ) {               \
        return e | flags;                                           \
    }                                                               \
    

#define ENUM_FLAG_VALUE(x) (1 << (x))