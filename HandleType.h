#pragma once

#include <type_traits>

#define _HANDLE_COMPARE_OPERATIONS(Type)                \
    friend bool operator == (Type lhs, Type rhs) {      \
        return lhs.handle == rhs.handle;                \
    }                                                   \
    friend bool operator != (Type lhs, Type rhs) {      \
        return lhs.handle != rhs.handle;                \
    }                                                   \
    friend bool operator < (Type lhs, Type rhs) {       \
        return lhs.handle < rhs.handle;                 \
    }                                                   \
    friend bool operator > (Type lhs, Type rhs) {       \
        return lhs.handle > rhs.handle;                 \
    }                                                   \
    friend bool operator <= (Type lhs, Type rhs) {      \
        return lhs.handle <= rhs.handle;                \
    }                                                   \
    friend bool operator >= (Type lhs, Type rhs) {      \
        return lhs.handle >= rhs.handle;                \
    }
    

 // Helper for adding "operator ->" if the Type is a pointer
template< typename Base, typename Type, bool pointer = std::is_pointer_v<Type> >
struct HandlePointerAccess {
    Type operator -> () {
        return static_cast<Base*>(this)->handle;
    }
    const Type operator -> () const {
        return static_cast<const Base*>(this)->handle;
    }
};

template< typename Base, typename Type >
struct HandlePointerAccess<Base, Type, false> {
};


struct BaseHandle {};

template< typename Type, typename Tag >
struct HandleType : public BaseHandle 
{
    using underlaying_type = Type;

    Type handle = Type(0);

    HandleType() = default;
    explicit HandleType( Type handle_ ) :
            handle(handle_)
    {}

    operator Type () const {
        return handle;
    }
    explicit operator bool () const {
        return handle != Type(0);
    }

    _HANDLE_COMPARE_OPERATIONS(HandleType);
};

#define MAKE_HANDLE( Name, Type ) struct _##Name##_tag; using Name = HandleType<Type, _##Name##_tag>


template< typename Type, typename tag,  typename Deleter, Type Null>
struct WrappedHandle :
    public HandlePointerAccess<WrappedHandle<Type, tag, Deleter, Null>, Type>
{
    Type handle = Null;

    WrappedHandle() = default;
    ~WrappedHandle()
    {
        if( handle != Null ) {
            Deleter()( handle );
        }
    }

    explicit WrappedHandle( Type handle_ ) : 
        handle(handle_)
    {}

    WrappedHandle( const WrappedHandle& ) = delete;
    WrappedHandle& operator = ( const WrappedHandle& ) = delete;
    
    WrappedHandle( WrappedHandle &&move ) noexcept(noexcept(std::swap(handle, move.handle)))
    {
        std::swap( handle, move.handle );
    }
    WrappedHandle& operator = ( WrappedHandle &&move ) noexcept(noexcept(std::swap(handle, move.handle)))
    {
        std::swap( handle, move.handle );
        return *this;
    }
    
    explicit operator bool () const {
        return handle != Null;
    }
    operator Type () const {
        return handle;
    }

    Type release() {
        Type tmp = handle;
        handle = Null;
        return tmp;
    }

    _HANDLE_COMPARE_OPERATIONS(WrappedHandle);
};

#define WRAP_HANDLE( Name, Type, Deleter, Null ) struct _##Name##_tag; using Name = WrappedHandle<Type, _##Name##_tag, Deleter, Null>
#define WRAP_HANDLE_FUNC( Name, Type, Func, Null )  \
    struct _##Name##_Deleter {                      \
        void operator () (Type val) noexcept {      \
            Func(val);                              \
        }                                           \
    };                                              \
    WRAP_HANDLE(Name, Type, _##Name##_Deleter, Null)


template< typename Type, typename tag,  typename Traits >
struct WrappedCStruct
{
    Type handle;

    WrappedCStruct() 
    {
        Traits::init(&handle);
    }
    ~WrappedCStruct()
    {
        Traits::free(&handle);
    }

    WrappedCStruct( const WrappedCStruct& ) = delete;
    WrappedCStruct& operator = ( const WrappedCStruct& ) = delete;
    
    WrappedCStruct( WrappedCStruct &&move ) = delete;
    WrappedCStruct& operator = ( WrappedCStruct &&move ) = delete;

    operator Type* () noexcept {
        return &handle;
    }
    Type& operator * () noexcept {
        return handle;
    }
    Type* operator -> () noexcept {
        return &handle;
    }
};

#define WRAP_CSTRUCT( Name, Type, Traits)                       \
    struct _##Name##_tag;                                       \
    using Name = WrappedCStruct<Type, _##Name##_tag, Traits>;

#define WRAP_CSTRUCT_MEMSET_INIT( Name, Type, Free )            \
    struct _##Name##_traits {                                   \
        static void init( Type *handle ) {                      \
            memset(handle, 0, sizeof(Type));                    \
        }                                                       \
        static void free( Type *handle ) {                      \
            Free(handle);                                       \
        }                                                       \
    };                                                          \
    WRAP_CSTRUCT(Name, Type, _##Name##_traits);



template< typename Type, typename Tag, typename Traits, Type Null >
struct RefcountedHandle :
    public HandlePointerAccess<RefcountedHandle<Type, Tag, Traits, Null>, Type>
{
    Type handle = Null;

    RefcountedHandle() = default;
    ~RefcountedHandle() {
        if( handle != Null ) {
            Traits::dec(handle);
        }
    }

    explicit RefcountedHandle( Type handle_ ) : 
        handle(handle_)
    {
        if (handle != Null) {
            Traits::inc(handle);
        }
    }

    RefcountedHandle( const RefcountedHandle &copy ) {
        if (copy.handle != Null) {
            Traits::inc(copy.handle);
            handle = copy.handle;
        }
    }
    RefcountedHandle& operator = ( const RefcountedHandle &copy )
    {
        if (handle != Null) {
            Traits::dec(handle);
            handle = Null;
        }
        if (copy.handle != Null) {
            Traits::inc(copy.handle);
            handle = copy.handle;
        }
        return *this;
    }
    
    RefcountedHandle( RefcountedHandle &&move ) {
        std::swap( handle, move.handle );
    }

    RefcountedHandle& operator = ( RefcountedHandle &&move ) {
        std::swap( handle, move.handle );
        return *this;
    }
    
    explicit operator bool () const {
        return handle != Null;
    }
    explicit operator Type () const {
        return handle;
    }
    
    _HANDLE_COMPARE_OPERATIONS(const RefcountedHandle&);
};

#define WRAP_REFCOUNTED_HANDLE(Name, Type, Traits, Null) struct _##Name##_tag; using Name = RefcountedHandle<Type, _##Name##_tag, Traits, Null>
#define WRAP_REFCOUNTED_HANDLE_FUNC(Name, Type, Inc, Dec, Null) \
    struct _##Name##_Traits {                                   \
        static void inc(Type handle) {                          \
            Inc(handle);                                        \
        }                                                       \
        static void dec(Type handle) {                          \
            Dec(handle);                                        \
        }                                                       \
    };                                                          \
    WRAP_REFCOUNTED_HANDLE(Name, Type, _##Name##_Traits, Null)

#undef _HANDLE_COMPARE_OPERATIONS