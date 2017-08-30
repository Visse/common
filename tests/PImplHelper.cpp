#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/PImplHelper.h"


TEST_CASE( "PImplHelper Basic", "[Common][PImplHelper]" )
{
    static int Constructions = 0,
               Destructions  = 0,
               Copies        = 0,
               Moves         = 0,
               CopyAssignments   = 0,
               MoveAssignments = 0;

    Constructions   = 0;
    Destructions    = 0;
    Copies          = 0;
    Moves           = 0;
    CopyAssignments     = 0;
    MoveAssignments = 0;

    struct Impl {
        void *_check = nullptr;
        int value = 0;

        bool valid() const {
            return _check == this;
        }

        Impl( int i = 0) :
            value(i)
        {
            _check = this;
            Constructions++;
        }
        ~Impl() {
            REQUIRE(valid());
            Destructions++;
        }

        Impl( const Impl &copy ) {
            REQUIRE(copy.valid());
            _check = this;
            Constructions++;
            Copies++;
            value = copy.value;
        }
        Impl( Impl &&move ) {
            REQUIRE(move.valid());
            _check = this;
            Constructions++;
            Moves++;
            value = move.value;
        }

        Impl& operator = ( const Impl &copy ) {
            REQUIRE(valid());
            REQUIRE(copy.valid());
            CopyAssignments++;
            return *this;
        }
        Impl& operator = ( Impl &&move ) {
            REQUIRE(valid());
            REQUIRE(move.valid());
            MoveAssignments++;
            return *this;
        }
    };
    using PImpl = PImplHelper<Impl, 16>;

    SECTION("Construction & destruction") {
        PImpl impl;

        Constructions--;
        Destructions--;
    }
    SECTION("Copying & Moving") {
        PImpl impl;
        PImpl copy(impl);
        PImpl moved(std::move(impl));

        impl = std::move(moved);
        copy = impl;

        Constructions -= 3;
        Destructions -= 3;
        Copies -= 1;
        Moves -= 1;
        CopyAssignments -= 1;
        MoveAssignments -= 1;
    }

    REQUIRE(Constructions   == 0);
    REQUIRE(Destructions    == 0);
    REQUIRE(Copies          == 0);
    REQUIRE(Moves           == 0);
    REQUIRE(CopyAssignments == 0);
    REQUIRE(MoveAssignments == 0);
}


TEST_CASE( "PImplHelper Virtual", "[Common][PImplHelper]" )
{
    static int BaseConstructions    = 0,
               BaseDestructions     = 0,
               BaseCopies           = 0,
               BaseMoves            = 0,
               BaseCopyAssignments  = 0,
               BaseMoveAssignments  = 0;

    static int Derived1Constructions    = 0,
               Derived1Destructions     = 0,
               Derived1Copies           = 0,
               Derived1Moves            = 0,
               Derived1CopyAssignments  = 0,
               Derived1MoveAssignments  = 0;

    static int Derived2Constructions    = 0,
               Derived2Destructions     = 0,
               Derived2Copies           = 0,
               Derived2Moves            = 0,
               Derived2CopyAssignments  = 0,
               Derived2MoveAssignments  = 0;


    struct BaseImpl {
        void *_check = nullptr;
        int value = 0;

        bool valid() const {
            return _check == this;
        }

        BaseImpl( int i = 0) :
            value(i)
        {
            _check = this;
            BaseConstructions++;
        }
        virtual ~BaseImpl() {
            REQUIRE(valid());
            BaseDestructions++;
        }

        BaseImpl( const BaseImpl &copy ) {
            REQUIRE(copy.valid());
            _check = this;
            BaseConstructions++;
            BaseCopies++;
            value = copy.value;
        }
        BaseImpl( BaseImpl &&move ) {
            REQUIRE(move.valid());
            _check = this;
            BaseConstructions++;
            BaseMoves++;
            value = move.value;
        }

        BaseImpl& operator = ( const BaseImpl &copy ) {
            REQUIRE(valid());
            REQUIRE(copy.valid());
            BaseCopyAssignments++;
            return *this;
        }
        BaseImpl& operator = ( BaseImpl &&move ) {
            REQUIRE(valid());
            REQUIRE(move.valid());
            BaseMoveAssignments++;
            return *this;
        }

        virtual int type() {
            return 0;
        }
    };
    using PImpl = PImplHelper<BaseImpl, 32, alignof(void*), PImplHelperFlags::AllocDerivedMove | PImplHelperFlags::AllowDerivedCopy>;

    struct Derived1Impl :
        public BaseImpl
    {
        void *_check = nullptr;

        bool valid() const {
            return _check == this;
        }

        Derived1Impl( int i = 0) :
            BaseImpl(i)
        {
            _check = this;
            Derived1Constructions++;
        }
        ~Derived1Impl() {
            REQUIRE(valid());
            Derived1Destructions++;
        }

        Derived1Impl( const Derived1Impl &copy ) {
            REQUIRE(copy.valid());
            _check = this;
            Derived1Constructions++;
            Derived1Copies++;
            value = copy.value;
        }
        Derived1Impl( Derived1Impl &&move ) {
            REQUIRE(move.valid());
            _check = this;
            Derived1Constructions++;
            Derived1Moves++;
            value = move.value;
        }

        Derived1Impl& operator = ( const Derived1Impl &copy ) {
            REQUIRE(valid());
            REQUIRE(copy.valid());
            Derived1CopyAssignments++;
            return *this;
        }
        Derived1Impl& operator = ( Derived1Impl &&move ) {
            REQUIRE(valid());
            REQUIRE(move.valid());
            Derived1MoveAssignments++;
            return *this;
        }

        virtual int type() {
            return 1;
        }
    };

    struct Derived2Impl :
        public BaseImpl
    {
        void *_check = nullptr;

        bool valid() const {
            return _check == this;
        }

        Derived2Impl( int i = 0) :
            BaseImpl(i)
        {
            _check = this;
            Derived2Constructions++;
        }
        ~Derived2Impl() {
            REQUIRE(valid());
            Derived2Destructions++;
        }

        Derived2Impl( const Derived2Impl &copy ) {
            REQUIRE(copy.valid());
            _check = this;
            Derived2Constructions++;
            Derived2Copies++;
            value = copy.value;
        }
        Derived2Impl( Derived2Impl &&move ) {
            REQUIRE(move.valid());
            _check = this;
            Derived2Constructions++;
            Derived2Moves++;
            value = move.value;
        }

        Derived2Impl& operator = ( const Derived2Impl &copy ) {
            REQUIRE(valid());
            REQUIRE(copy.valid());
            Derived2CopyAssignments++;
            return *this;
        }
        Derived2Impl& operator = ( Derived2Impl &&move ) {
            REQUIRE(valid());
            REQUIRE(move.valid());
            Derived2MoveAssignments++;
            return *this;
        }
    
        virtual int type() {
            return 2;
        }
    };

    SECTION("Construction")
    {
        PImpl impl;
        PImpl impl1((PImpl::DerivedConstructorTag<Derived1Impl>()));
        PImpl impl2((PImpl::DerivedConstructorTag<Derived2Impl>()));

        REQUIRE(impl->type()  == 0);
        REQUIRE(impl1->type() == 1);
        REQUIRE(impl2->type() == 2);

        BaseConstructions -= 3;
        BaseDestructions  -= 3;

        Derived1Constructions -= 1;
        Derived1Destructions  -= 1;

        Derived2Constructions -= 1;
        Derived2Destructions  -= 1;
    }

    SECTION("Copy & Movement")
    {
        PImpl impl;
        PImpl impl1((PImpl::DerivedConstructorTag<Derived1Impl>()));
        PImpl impl2((PImpl::DerivedConstructorTag<Derived2Impl>()));

        REQUIRE(impl->type()  == 0);
        REQUIRE(impl1->type() == 1);
        REQUIRE(impl2->type() == 2);


        impl = impl1;
        impl1 = std::move(impl2);
        impl2 = PImpl();

        REQUIRE(impl->type() == 1);
        REQUIRE(impl1->type() == 2);
        REQUIRE(impl2->type() == 0);

        BaseConstructions -= 7;
        BaseDestructions  -= 7;
        BaseMoves         -= 1;

        Derived1Constructions -= 2;
        Derived1Destructions  -= 2;
        Derived1Copies        -= 1;

        Derived2Constructions -= 2;
        Derived2Destructions  -= 2;
        Derived2Moves         -= 1;
    }

    REQUIRE(BaseConstructions   == 0);
    REQUIRE(BaseDestructions    == 0);
    REQUIRE(BaseCopies          == 0);
    REQUIRE(BaseMoves           == 0);
    REQUIRE(BaseCopyAssignments == 0);
    REQUIRE(BaseMoveAssignments == 0);

    REQUIRE(Derived1Constructions   == 0);
    REQUIRE(Derived1Destructions    == 0);
    REQUIRE(Derived1Copies          == 0);
    REQUIRE(Derived1Moves           == 0);
    REQUIRE(Derived1CopyAssignments == 0);
    REQUIRE(Derived1MoveAssignments == 0);
    
    REQUIRE(Derived2Constructions   == 0);
    REQUIRE(Derived2Destructions    == 0);
    REQUIRE(Derived2Copies          == 0);
    REQUIRE(Derived2Moves           == 0);
    REQUIRE(Derived2CopyAssignments == 0);
    REQUIRE(Derived2MoveAssignments == 0);
}
