#pragma once


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
        
    friend bool operator == ( HandleType lhs, HandleType rhs ) {
        return lhs.handle == rhs.handle;
    }
    friend bool operator != ( HandleType lhs, HandleType rhs ) {
        return lhs.handle != rhs.handle;
    }
    friend bool operator < ( HandleType lhs, HandleType rhs ) {
        return lhs.handle < rhs.handle;
    }
};

#define MAKE_HANDLE( Name, Type ) struct _##Name##_tag; using Name = HandleType<Type, _##Name##_tag>



template< typename Type, typename tag,  typename Deleter, Type Null>
struct WrappedHandle {
    Type handle = Null;

    WrappedHandle() = default;
    ~WrappedHandle() {
        if( handle != Null ) {
            Deleter()( handle );
        }
    }

    explicit WrappedHandle( Type handle_ ) : 
        handle(handle_)
    {}

    WrappedHandle( const WrappedHandle& ) = delete;
    WrappedHandle& operator = ( const WrappedHandle& ) = delete;
    
    WrappedHandle( WrappedHandle &&move ) {
        std::swap( handle, move.handle );
    }
    WrappedHandle& operator = ( WrappedHandle &&move ) {
        std::swap( handle, move.handle );
        return *this;
    }
    
    operator bool () const {
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

};


template< typename Type, typename tag,  typename Deleter, Type *Null>
struct WrappedHandle<Type*,tag, Deleter, Null> {
    Type *handle = Null;

    WrappedHandle() = default;
    ~WrappedHandle() {
        if( handle != Null ) {
            Deleter()( handle );
        }
    }

    explicit WrappedHandle( Type *handle_ ) : 
        handle(handle_)
    {}

    WrappedHandle( const WrappedHandle& ) = delete;
    WrappedHandle& operator = ( const WrappedHandle& ) = delete;

    WrappedHandle( WrappedHandle &&move ) {
        std::swap( handle, move.handle );
    }
    WrappedHandle& operator = ( WrappedHandle &&move ) {
        std::swap( handle, move.handle );
        return *this;
    }

    operator bool () const {
        return handle != Null;
    }
    operator Type* () const {
        return handle;
    }

    Type release() {
        Type tmp = handle;
        handle = Null;
        return tmp;
    }

    Type* operator -> () {
        return handle;
    }
};


#define WRAP_HANDLE( Name, Type, Deleter, Null ) struct _##Name##_tag; using Name = WrappedHandle<Type, _##Name##_tag, Deleter, Null>
#define WRAP_HANDLE_FUNC( Name, Type, Func, Null )  \
    struct _##Name##_Deleter {                      \
        void operator () (Type val) {               \
            Func(val);                              \
        }                                           \
    };                                              \
    WRAP_HANDLE(Name, Type, _##Name##_Deleter, Null)


template< typename Type, typename Tag, typename Traits, Type Null >
struct RefcountedHandle {
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
    
    operator bool () const {
        return handle != Null;
    }
    explicit operator Type () const {
        return handle;
    }

    friend bool operator == ( const RefcountedHandle &lhs, const RefcountedHandle &rhs ) {
        return lhs.handle == rhs.handle;
    }
    friend bool operator != ( const RefcountedHandle &lhs, const RefcountedHandle &rhs ) {
        return lhs.handle != rhs.handle;
    }
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
