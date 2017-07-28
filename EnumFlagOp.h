#pragma once

#include <type_traits>

#define DECLARE_ENUM_FLAG( enum )                       \
    inline enum operator | ( enum e1, enum e2 ) {       \
        return static_cast<enum>(                       \
            std::underlying_type<enum>::type(e1) |      \
            std::underlying_type<enum>::type(e2)        \
        );                                              \
    }                                                   \
    inline enum operator & ( enum e1, enum e2 ) {       \
        return static_cast<enum>(                       \
            std::underlying_type<enum>::type(e1) &      \
            std::underlying_type<enum>::type(e2)        \
        );                                              \
    }                                                   \
    inline enum operator ^ ( enum e1, enum e2 ) {       \
        return static_cast<enum>(                       \
            std::underlying_type<enum>::type(e1) ^      \
            std::underlying_type<enum>::type(e2)        \
        );                                              \
    }                                                   \
    inline enum operator ~ ( enum e1 ) {                \
        return static_cast<enum>(                       \
            ~std::underlying_type<enum>::type(e1)       \
        );                                              \
    }                                                   \
    inline bool any( enum e, enum flags ) {             \
        return std::underlying_type<enum>::type(e&flags) != 0; \
    }                                                   \
    inline bool all( enum e, enum flags ) {             \
        return (e&flags) == flags;                      \
    }                                                   \
    inline bool none( enum e, enum flags ) {            \
        return std::underlying_type<enum>::type(e&flags) == 0; \
    }                                                   \
    inline enum clear( enum e, enum flags ) {           \
        return e & (~flags);                            \
    }                                                   \
    inline enum set( enum e, enum flags ) {             \
        return e | flags;                               \
    }                                                   \
    

