#pragma once

#include <type_traits>
#include <new>

#include "EnumFlagOp.h"

enum class PImplHelperFlags {
    None = 0,
    // Allow derived types of impl
    AllowDerivedImpl = 1,
    // Allow copy constructor & assignment of derived types 
    //   (otherwise these are not avalible if AllowDerivedImpl is specified)
    //   Only the copy constructor is used (even if its a assignment)
    AllowDerivedCopy = AllowDerivedImpl | 2,
    // Allow move constructor & assignment of derived types 
    //   (otherwise these are not avalible if AllowDerivedImpl is specified)
    //   Only the move constructor is used (even if its a assignment)
    AllocDerivedMove = AllowDerivedImpl | 4,
};
DECLARE_ENUM_FLAG(PImplHelperFlags);


namespace internal
{
    // Tag to allow for construction of derived types
    template< typename Type >
    struct DerivedConstructorTag {};

    // Helper class
    template< typename Impl, size_t Size, size_t Alignment, PImplHelperFlags Flags >
    struct PImplHelperInternals
    {
        // Used if AllowDerivedCopy is not specified
        template< typename Base >
        struct BaseCopy {
            template< typename DerivedType >
            void initCopy() {}
            
            void onMove( Impl *to, const BaseCopy &move ) {}

            void doCopyConstruct( Impl *to, const BaseCopy &copy ) {
                new (to) Impl(*static_cast<const Base&>(copy).impl());
            }

            void doCopyAssign( Impl *to, const BaseCopy &copy ) {
                *to = *static_cast<const Base&>(copy).impl();
            }
        };
        
        // Used if AllowDerivedMove is not specified
        template< typename Base >
        struct BaseMove {
            template< typename DerivedType >
            void initMove() {}

            void onCopy( Impl *to, const BaseMove &copy ) {}
            void doMoveConstruct( Impl *to, BaseMove &&move ) {
                new (to) Impl(std::move(*static_cast<Base&>(move).impl()));
            }
            void doMoveAssign( Impl *to, BaseMove &&move ) {
                *to = std::move(*static_cast<Base&>(move).impl());
            }
        };

        // Used if AllowDerivedCopy is specified 
        template< typename Base >
        struct AllowDerivedCopy {
            template< typename Type >
            static void CopyFunction( Impl *to, const Impl *from ) {
                new (to) Type(static_cast<const Type&>(*from));
            }
            // this is our copy function - its set on construction to the correct type
            // and is used to call the copy-constructor of derived types
            void (*copyFunction)( Impl *to, const Impl *from ) = CopyFunction<Impl>;
            
            AllowDerivedCopy() = default;

            template< typename DerivedType >
            void initCopy() {
                copyFunction = CopyFunction<DerivedType>;
            }

            void doCopyConstruct( Impl *to, const AllowDerivedCopy &copy ) {
                copyFunction = copy.copyFunction;
                copyFunction(to, static_cast<const Base&>(copy).impl());
            }

            void doCopyAssign( Impl *to, const AllowDerivedCopy &copy ) {
                to->~Impl();
                doCopyConstruct(to, copy);
            }

            void onMove( Impl *to, const AllowDerivedCopy &move ) {
                copyFunction = move.copyFunction;
            }
        };

        template< typename Base >
        struct AllowDerivedMove {
            template< typename Type >
            static void MoveFunction( Impl *to, Impl *from ) {
                new (to) Type(static_cast<Type&&>(*from));
            }
            // This is our move function - its set on construction to the correct type
            // and is used to call the move-constructor of derived types.
            void (*moveFunction)( Impl *to, Impl *from ) = MoveFunction<Impl>;

            AllowDerivedMove() = default;
            
            template< typename DerivedType >
            void initMove() {
                moveFunction = MoveFunction<DerivedType>;
            }
            
            void onCopy( Impl *to, const AllowDerivedMove &copy ) {
                moveFunction = copy.moveFunction;
            }
            void doMoveConstruct( Impl *to, AllowDerivedMove &&move ) {
                moveFunction = move.moveFunction;
                moveFunction(to, static_cast<Base&>(move).impl());
            }
            void doMoveAssign( Impl *to, AllowDerivedMove &&move ) {
                to->~Impl();
                doMoveConstruct(to, std::move(move));
            }
        };


        // Selct the correct class based on flags
        template< typename Base >
        using CopyFunction = typename std::conditional<all(Flags, PImplHelperFlags::AllowDerivedCopy), AllowDerivedCopy<Base>, BaseCopy<Base>>::type;
        
        template< typename Base >
        using MoveFunction = typename std::conditional<all(Flags, PImplHelperFlags::AllocDerivedMove), AllowDerivedMove<Base>, BaseMove<Base>>::type;

        struct Base :
            private CopyFunction<Base>,
            private MoveFunction<Base>
        {
            typename std::aligned_storage<Size, Alignment>::type mImpl;

            Base() {
                new (impl()) Impl;
            }

            template< typename... Args >
            Base( Args&&... args ) {
                new (impl()) Impl(std::forward<Args>(args)...);
            }

            template< typename DerivedType, typename... Args >
            Base( DerivedConstructorTag<DerivedType>, Args&&... args )
            {
                // Only allow derived types if AllowDerivedImpl is specified
                static_assert(all(Flags, PImplHelperFlags::AllowDerivedImpl), "PImplHelper not compiled with support for derived types!");
                
                initCopy<DerivedType>();
                initMove<DerivedType>();
                new (impl()) DerivedType(std::forward<Args>(args)...);
            }

            Base( const Base &copy ) {
                // Only copy construction if if either AllowDerivedCopy is specified, or AllowDerivedImpl is not
                static_assert(none(Flags, PImplHelperFlags::AllowDerivedImpl) || all(Flags, PImplHelperFlags::AllowDerivedCopy), "PImplHelper not compiled to support derived classes and copy functions");
                onCopy(impl(), copy);
                doCopyConstruct(impl(), copy);
            }

            Base( Base &&move ) {
                // Only move construction if if either AllocDerivedMove is specified, or AllowDerivedImpl is not
                static_assert(none(Flags, PImplHelperFlags::AllowDerivedImpl) || all(Flags, PImplHelperFlags::AllocDerivedMove), "PImplHelper not compiled to support derived classes and move functions");
                onMove(impl(), move);
                doMoveConstruct(impl(), std::move(move));
            }

            ~Base()
            {
                impl()->~Impl();
            }
            
            Base& operator = ( const Base &copy ) {
                // Only copy assignment if if either AllowDerivedCopy is specified, or AllowDerivedImpl is not
                static_assert(none(Flags, PImplHelperFlags::AllowDerivedImpl) || all(Flags, PImplHelperFlags::AllowDerivedCopy), "PImplHelper not compiled to support derived classes and copy functions");
                onCopy(impl(), copy);
                doCopyAssign(impl(), copy);
                return *this;
            }

            Base& operator = ( Base &&move ) {
                // Only move assignment if if either AllocDerivedMove is specified, or AllowDerivedImpl is not
                static_assert(none(Flags, PImplHelperFlags::AllowDerivedImpl) || all(Flags, PImplHelperFlags::AllocDerivedMove), "PImplHelper not compiled to support derived classes and move functions");
                onMove(impl(), move);
                doMoveAssign(impl(), std::move(move));
                return *this;
            }

            Impl* impl() {
                return (Impl*)&mImpl;
            }

            const Impl* impl() const {
                return (const Impl*)&mImpl;
            }
        };
    };
    
    template< typename Impl, size_t Size, size_t Alignment, PImplHelperFlags Flags >
    using PImplHelperBase = typename PImplHelperInternals<Impl, Size, Alignment, Flags>::Base;   
}

#include "compiler_warnings/push.h"
#include "compiler_warnings/ignore_multiple_copy_constructors.h"
template< typename Impl, size_t Size, size_t Alignment=alignof(void*), PImplHelperFlags Flags = PImplHelperFlags::None>
class PImplHelper :
    private internal::PImplHelperBase<Impl, Size, Alignment, Flags>
{
    using Base = internal::PImplHelperBase<Impl, Size, Alignment, Flags>;

    template< size_t RealSize = sizeof(Impl), size_t RealAlignment=alignof(Impl) >
    struct CheckSizeAndAlign {
        static_assert(RealSize > 0, "Impl must be defined!");
        static_assert(RealSize <= Size, "The specified size is to small for the implementation!");
        static_assert(RealAlignment <= Alignment, "The specified alignment is to small for the implementation!");
    };

public:
    using Base::impl;

    template< typename DerivedType >
    using DerivedConstructorTag = internal::DerivedConstructorTag<DerivedType>;
    
    PImplHelper() : 
        Base() 
    {
        CheckSizeAndAlign<>();
    }

    PImplHelper( const PImplHelper &copy ) :
        Base(static_cast<const Base&>(copy))
    {
        CheckSizeAndAlign<>();
    }

    // Need a non const copy constructor, otherwise
    // the variadic templated one will be used.
    PImplHelper( PImplHelper &copy ) :
        Base(static_cast<const Base&>(copy))
    {
        CheckSizeAndAlign<>();
    }

    PImplHelper( PImplHelper &&move ) :
        Base(static_cast<Base&&>(move))
    {
        CheckSizeAndAlign<>();
    }

    template< typename... Args >
    PImplHelper( Args&&... args ) :
        Base(std::forward<Args>(args)...)
    {
        CheckSizeAndAlign<>();
    }

    template< typename DerivedType, typename... Args >
    PImplHelper( DerivedConstructorTag<DerivedType> tag, Args&&... args ) :
        Base(tag, std::forward<Args>(args)...)
    {
        static_assert(std::is_base_of<Impl, DerivedType>::value, "DerivedType isn't derived from Impl!");
        static_assert(std::has_virtual_destructor<Impl>::value, "Impl must have a virtual destructor!");
        
        CheckSizeAndAlign<sizeof(DerivedType), alignof(DerivedType)>();
    }

    ~PImplHelper() {}

    PImplHelper& operator = ( const PImplHelper& ) = default;
    PImplHelper& operator = ( PImplHelper&& ) = default;

    operator Impl* () {
        return impl();
    }
    
    operator const Impl* () const {
        return impl();
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
#include "compiler_warnings/pop.h"
