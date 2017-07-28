#pragma once

#include "PPUtils.h"
#include "Throw.h"

#include <stdexcept>
#include <utility>
#include <new>
#include <tuple>

namespace internal {
    template< typename Type >
    void destroy( Type &type ) {
        type.~Type();
    }
    template< typename Type, typename ... Args >
    void construct( Type &type, Args&&... args ) {
        new (&type) Type(std::forward<Args>(args)...);
    }
}

#define _MAKE_VARIANT_TYPE_ENUM_HELPER( Name, DataType) Name,
#define _MAKE_VARIANT_TYPE_ENUM( Name, Args ) _MAKE_VARIANT_TYPE_ENUM_HELPER Args

#define _MAKE_VARIANT_TYPE_UNION_HELPER( Name, DataType ) struct { Type type; DataType data; } Name;
#define _MAKE_VARIANT_TYPE_UNION( Name, Args ) _MAKE_VARIANT_TYPE_UNION_HELPER Args

#define _MAKE_VARIANT_DESTROY_HELPER( Name, DataType ) case Type::Name: ::internal::destroy(Name.data); type = Type::None; break;
#define _MAKE_VARIANT_DESTROY( Name, Args ) _MAKE_VARIANT_DESTROY_HELPER Args

#define _MAKE_VARIANT_ASSIGN_HELPER( Name, DataType )   \
    template< typename... Args >                        \
    void assign ## Name ( Args&&... args ) {            \
        destroy();                                      \
        ::internal::construct(Name.data, std::forward<Args>(args)... ); \
        type = Type :: Name;                            \
    }
#define _MAKE_VARIANT_ASSIGN( Name, Args ) _MAKE_VARIANT_ASSIGN_HELPER Args

#define _MAKE_VARIANT_MEMBER_FUNCTION_HELPER( TypeName, Name, DataType )    \
    DataType& get ## Name () {                                              \
        if (mData.type != Type::Name)                                       \
            THROW(std::runtime_error, "Variant \"%s\" doesn't contain \"%s\" (%s) - current type %i", #TypeName, #Name, #DataType, (int)mData.type); \
        return mData.Name.data;                                             \
    }                                                                       \
    template< typename... Args >                                            \
    void set ## Name( Args&&... args ) {                                    \
        mData.assign ## Name ( std::forward<Args>(args)... );               \
    }

#define _MAKE_VARIANT_UNPACK( Name, DataType ) Name, DataType
#define _MAKE_VARIANT_EXPAND( Func, Args) Func Args
#define _MAKE_VARIANT_MEMBER_FUNCTION( Name, Args ) _MAKE_VARIANT_EXPAND( _MAKE_VARIANT_MEMBER_FUNCTION_HELPER, ( Name, _MAKE_VARIANT_UNPACK Args) )

#define _MAKE_VARIANT_APPLY_HELPER( Name, DataType ) case Type::Name: func(mData.Name.data); break;
#define _MAKE_VARIANT_APPLY( Name, Args ) _MAKE_VARIANT_APPLY_HELPER Args

#define _MAKE_VARIANT_CONSTRUCT_HELPER( Name, DataType ) case Type::Name: set##Name(std::forward<Args>(args)...); break;
#define _MAKE_VARIANT_CONSTRUCT( Name, Args ) _MAKE_VARIANT_CONSTRUCT_HELPER Args

#define _MAKE_VARIANT_TUPLE_HELPER( Name, DataType ) , DataType
#define _MAKE_VARIANT_TUPLE( Name, Args ) _MAKE_VARIANT_TUPLE_HELPER Args

#define MAKE_VARIANT( Name, ... )       \
    class Name {                        \
        using ThisType = Name;          \
    public:                             \
        enum Type {                     \
            None = 0,                   \
            PP_UTILS_MAP(Name, _MAKE_VARIANT_TYPE_ENUM, __VA_ARGS__) \
        };                              \
        Name() = default;               \
        ~Name() = default;              \
        template< typename... Args >    \
        Name( Type type, Args&&... args) {  \
            switch (type) {             \
            case Type::None: break;     \
            PP_UTILS_MAP(Name, _MAKE_VARIANT_CONSTRUCT, __VA_ARGS__) \
            }                           \
        }                               \
        Type type() const {             \
            return mData.type;          \
        }                               \
        void clear() {                  \
            mData.destroy();            \
        }                               \
        template< typename Func >       \
        void apply( Func &&func ) {     \
            switch(mData.type) {        \
            case Type::None: break;     \
            PP_UTILS_MAP(Name, _MAKE_VARIANT_APPLY, __VA_ARGS__) \
            }                           \
        }                               \
        PP_UTILS_MAP(Name, _MAKE_VARIANT_MEMBER_FUNCTION, __VA_ARGS__) \
        using VariantTuple = std::tuple<void PP_UTILS_MAP(Name, _MAKE_VARIANT_TUPLE, __VA_ARGS__)>; \
        template< Type type >           \
        using underlaying_type = std::tuple_element_t<(int)type, VariantTuple>; \
    private:                            \
        union DataUnion {               \
            Type type;                  \
            PP_UTILS_MAP(Name, _MAKE_VARIANT_TYPE_UNION, __VA_ARGS__) \
            DataUnion() :               \
                type(Type::None)        \
            {}                          \
            ~DataUnion() {              \
                destroy();              \
            }                           \
            void destroy() {            \
                switch(type) {          \
                case Type::None: break; \
                PP_UTILS_MAP(Name, _MAKE_VARIANT_DESTROY, __VA_ARGS__) \
                }                       \
            }                           \
            PP_UTILS_MAP(Name, _MAKE_VARIANT_ASSIGN, __VA_ARGS__) \
        } mData;                        \
    }

