#pragma once

#include <type_traits>
#include <new>


template< typename Impl, size_t Size, size_t Alignment=alignof(void*) >
class PImplHelper {
    typename std::aligned_storage<Size, Alignment>::type mImpl;
    template< size_t RealSize = sizeof(Impl), size_t RealAlignment=alignof(Impl) >
    struct CheckSizeAndAlign {
        static_assert(RealSize <= Size, "The specified size is to small for the implementation!");
        static_assert(RealAlignment <= Alignment, "The specified alignment is to small for the implementation!");
    };
    

public:
    template< typename... Args >
    PImplHelper( Args&&... args ) {
        CheckSizeAndAlign<>();
        new (impl()) Impl(std::forward<Args>(args)...);
    }
    
    template< typename Type >
    struct DerivedConstructorTag {};

    template< typename DerivedType, typename... Args>
    PImplHelper( DerivedConstructorTag<DerivedType>, Args&&... args ) {
        static_assert(std::is_base_of<Impl, DerivedType>::value, "DerivedType isn't derived from Impl!");
        static_assert(std::has_virtual_destructor<Impl>::value, "Impl must have a virtual destructor!");
        
        CheckSizeAndAlign<sizeof(DerivedType), alignof(DerivedType)>();

        new (impl()) DerivedType(std::forward<Args>(args)...);
    }

    ~PImplHelper() {
        impl()->~Impl();
    }

    PImplHelper( const PImplHelper &copy ) :
        PImplHelper(*copy.impl())    
    {}
    PImplHelper( PImplHelper &&move) :
        PImplHelper(std::move(*move.impl()))
    {}

    PImplHelper& operator = ( const PImplHelper &copy ) {
        *impl() = *copy.impl();
        return *this;
    }
    PImplHelper& operator = ( PImplHelper &&move ) {
        *impl() = std::move(*move.impl());
        return *this;
    }
    
    operator Impl* () {
        return impl();
    }
    
    operator const Impl* () const {
        return impl();
    }

    Impl* impl() {
        return (Impl*)&mImpl;
    }

    const Impl* impl() const {
        return (const Impl*)&mImpl;
    }

    Impl* operator -> () {
        return impl();
    }

    const Impl* operator -> () const {
        return impl();
    }

    Impl& operator * () {
        return *impl();
    }

    const Impl& operator * () const {
        return *impl();
    }
};

