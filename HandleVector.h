#pragma once


#include "HandleType.h"
#include "IteratorAdopter.h"

#include <vector>
#include <type_traits>
#include <cassert>

static constexpr const unsigned HANDLE_VECTOR_DEFAULT = (unsigned)-1;
template< typename Handle, typename Value, unsigned GenerationBits_=HANDLE_VECTOR_DEFAULT, unsigned DataBits_=HANDLE_VECTOR_DEFAULT, typename Handle::underlaying_type DataValue_=0, bool ManualHandels_=false > 
class HandleVector {
public:
    using underlaying_type = typename Handle::underlaying_type;
    using key_vector = std::vector<underlaying_type>;
    
    static constexpr const unsigned TypeBits = CHAR_BIT * sizeof (underlaying_type);
    static constexpr const unsigned GenerationBits = (GenerationBits_ == HANDLE_VECTOR_DEFAULT) ? 8 : GenerationBits_;
    static constexpr const unsigned DataBits = (DataBits_ == HANDLE_VECTOR_DEFAULT) ? 1 : DataBits_;
    static constexpr const unsigned IndexBits = TypeBits - GenerationBits - DataBits;
    static constexpr const bool ManualHandles = ManualHandels_;

    static constexpr const size_t MaxValues = (1 << IndexBits)-1;
    static constexpr const underlaying_type IndexMask = (1 << IndexBits) - 1;
    static constexpr const underlaying_type GenerationMask = (1 << GenerationBits) - 1;
    static constexpr const underlaying_type DataMask = (1 << DataBits) -1;

    static constexpr const underlaying_type DataValue = DataValue_ & DataMask;

    static constexpr const unsigned IndexOffset = 0;
    static constexpr const unsigned GenerationOffset = IndexBits;
    static constexpr const unsigned DataOffset = IndexBits + GenerationBits;

    static_assert (std::is_base_of<BaseHandle, Handle>::value, "HandleType isn't a handle!");
    static_assert ((GenerationBits+DataBits) < TypeBits, "Invalid number of GenerationBits and DataBits!");
    static_assert ((DataValue_&~DataMask) == 0 || DataValue_ == HANDLE_VECTOR_DEFAULT, "Invalid DataValue, doesn't match the specifed DataBits!");
    static_assert (DataBits > 0, "Atleast 1 DataBit is neaded!, (for internal bookkepping)");


    static constexpr bool IsHandleFromThis(Handle handle) {
        return GetData(handle) == DataValue;
    }
    

    template< typename Type >
    class IteratorBase;

    typedef IteratorBase<Value> iterator;
    typedef IteratorBase<const Value> const_iterator;


private:
    static constexpr underlaying_type MaskAndOffset( underlaying_type value, underlaying_type mask, underlaying_type offset ) {
        return (value&mask) << offset;
    }

    static constexpr underlaying_type OffsetAndMask( underlaying_type value, underlaying_type mask, underlaying_type offset ) {
        return (value>>offset) & mask;
    }

    static constexpr underlaying_type GetIndex( Handle handle ) {
        return OffsetAndMask((underlaying_type)handle, IndexMask, IndexOffset);
    }
    static constexpr underlaying_type GetGeneration( Handle handle ) {
        return OffsetAndMask((underlaying_type)handle, GenerationMask, GenerationOffset);
    }
    static constexpr underlaying_type GetData( Handle handle ) {
        return OffsetAndMask((underlaying_type)handle, DataMask, DataOffset);
    }
    static constexpr Handle CreateHandle( underlaying_type generation, underlaying_type index, underlaying_type data ) {
        return Handle(MaskAndOffset(data, DataMask, DataOffset) |
                      MaskAndOffset(generation, GenerationMask, GenerationOffset) |
                      MaskAndOffset(index, IndexMask, IndexOffset)
        );
    }
    static constexpr Handle SetIndex( Handle handle, underlaying_type index ) {
        return Handle((underlaying_type(handle)&~IndexMask) | 
                       MaskAndOffset(index,IndexMask,IndexOffset)
        );  
    }

    static constexpr const underlaying_type FreeDataValue = DataValue ^ DataMask;
    // Handle must be from this container
    static constexpr bool IsHandleFree( Handle handle ) {
        return GetData(handle) == FreeDataValue;
    }

    struct ValuePairStorage {
        Handle handle = CreateHandle(0, 0, FreeDataValue);
        std::aligned_union_t<0, Value> storage;
        
        ~ValuePairStorage() noexcept(std::is_nothrow_destructible_v<Value>) {
            if (isValid()) {
                destroy();
            }
        }

        bool isValid() const {
            return IsHandleFree(handle) == false;
        }

        Value* value() {
            return reinterpret_cast<Value*>(&storage);
        }
        const Value* value() const {
            return reinterpret_cast<const Value*>(&storage);
        }
        template< typename... Args >
        void construct( Args&&... args ) noexcept(std::is_nothrow_constructible_v<Value, Args...>) {
            new(value()) Value(std::forward<Args>(args)...);
        }
        void destroy() {
            value()->~Value();
        }
    };

    template< typename Type, bool Enable=std::is_copy_constructible_v<Value> >
    struct CopyConstructable {
        CopyConstructable() = default;
        CopyConstructable( CopyConstructable&& ) = default;
        CopyConstructable& operator = ( const CopyConstructable& ) = default;
        CopyConstructable& operator = ( CopyConstructable&& ) = default;

        Type* base() {
            return static_cast<Type*>(this);
        }
        const Type* base() const {
            return static_cast<const Type*>(this);
        }
        
        CopyConstructable( const CopyConstructable &copy ) noexcept(std::is_nothrow_copy_constructible_v<Value>)
        {
            auto base = this->base();
            auto other = copy.base();
            base->handle = other->handle;
            if (other->isValid()) {
                base->construct(*other->value());
            }
        }
        
    };
    
    template< typename Type >
    struct CopyConstructable<Type, false> {
        CopyConstructable() = default;
        CopyConstructable( const CopyConstructable& ) = delete;
        CopyConstructable( CopyConstructable&& ) = default;
        CopyConstructable& operator = ( const CopyConstructable& ) = default;
        CopyConstructable& operator = ( CopyConstructable&& ) = default;
    };
    
    template< typename Type, bool Enable=std::is_copy_assignable_v<Value>>
    struct CopyAssignable {
        CopyAssignable() = default;
        CopyAssignable( const CopyAssignable& ) = default;
        CopyAssignable( CopyAssignable&& ) = default;
        CopyAssignable& operator = ( CopyAssignable&& ) = default;

        Type* base() {
            return static_cast<Type*>(this);
        }
        const Type* base() const {
            return static_cast<const Type*>(this);
        }

        CopyAssignable& operator = ( const CopyAssignable &copy ) noexcept(std::is_nothrow_copy_constructible_v<Value> && std::is_nothrow_copy_assignable_v<Value>)
        {
            auto base = this->base();
            auto other = copy.base();

            if (base->isValid() && other->isValid()) {
                *base->value() = *other->value();
            }
            else if (base->isValid()) {
                base->destroy();
            }
            else {
                base->construct(*other->value());
            }
            base->handle = other->handle;
            return *this;
        }
    };
    
    template< typename Type >
    struct CopyAssignable<Type, false> {
        CopyAssignable() = default;
        CopyAssignable( const CopyAssignable& ) = default;
        CopyAssignable( CopyAssignable&& ) = default;
        CopyAssignable& operator = ( const CopyAssignable &copy ) = delete;
        CopyAssignable& operator = ( CopyAssignable&& ) = default;
    };

    template< typename Type, bool Enable=std::is_move_constructible_v<Value> >
    struct MoveConstructable {
        MoveConstructable() = default;
        MoveConstructable( const MoveConstructable& ) = default;
        MoveConstructable& operator = ( const MoveConstructable &copy ) = default;
        MoveConstructable& operator = ( MoveConstructable&& ) = default;

        Type* base() {
            return static_cast<Type*>(this);
        }
        const Type* base() const {
            return static_cast<const Type*>(this);
        }

        MoveConstructable( MoveConstructable &&move ) noexcept(std::is_nothrow_move_constructible_v<Value>)
        {
            auto base = this->base();
            auto other = move.base();
            base->handle = other->handle;
            if (other->isValid()) {
                base->construct(std::move(*other->value()));
            }
        }
    };
    
    template< typename Type >
    struct MoveConstructable<Type, false> {
        MoveConstructable() = default;
        MoveConstructable( const MoveConstructable& ) = default;
        MoveConstructable( MoveConstructable&& ) = delete;
        MoveConstructable& operator = ( const MoveConstructable& ) = default;
        MoveConstructable& operator = ( MoveConstructable&& ) = default;
    };
    
    template< typename Type, bool Enable=std::is_move_assignable_v<Value> >
    struct MoveAssignable {
        MoveAssignable() = default;
        MoveAssignable( const MoveAssignable& ) = default;
        MoveAssignable( MoveAssignable&& ) = default;
        MoveAssignable& operator = ( const MoveAssignable &copy ) = default;

        Type* base() {
            return static_cast<Type*>(this);
        }
        const Type* base() const {
            return static_cast<const Type*>(this);
        }

        MoveAssignable& operator = ( MoveAssignable &&move ) noexcept(std::is_nothrow_move_constructible_v<Value> && std::is_nothrow_move_assignable_v<Value>)
        {
            auto base = this->base();
            auto other = move.base();

            if (base->isValid() && other->isValid()) {
                *base->value() = std::move(*other->value());
            }
            else if (base->isValid()) {
                base->destroy();
            }
            else {
                base->construct(std::move(*other->value()));
            }
            base->handle = other->handle;
            return *this;
        }
    };

    template< typename Type >
    struct MoveAssignable<Type, false> {
        MoveAssignable() = default;
        MoveAssignable( const MoveAssignable& ) = default;
        MoveAssignable( MoveAssignable&& ) = default;
        MoveAssignable& operator = ( const MoveAssignable& ) = default;
        MoveAssignable& operator = ( const MoveAssignable&& ) = delete;
    };


    struct ValuePairBase :
        public ValuePairStorage,
        public CopyConstructable<ValuePairBase>,
        public CopyAssignable<ValuePairBase>,
        public MoveConstructable<ValuePairBase>,
        public MoveAssignable<ValuePairBase>
    {
        static_assert( std::is_copy_constructible_v<Value> || std::is_move_constructible_v<Value>, "Value must be either copyable or movable");
        
        ValuePairBase() = default;
        ValuePairBase( const ValuePairBase& ) = default;
        ValuePairBase( ValuePairBase&& ) = default;
        ValuePairBase& operator = ( const ValuePairBase& ) = default;
        ValuePairBase& operator = ( ValuePairBase&& ) = default;
    };

    struct ValuePair :
        public ValuePairBase
    {
        using ValuePairBase::ValuePairBase;

        template< typename... Args >
        ValuePair( Handle handle, Args&&... args ) noexcept(std::is_nothrow_constructible_v<Value, Args...>)
        {
            this->handle = handle;
            construct(std::forward<Args>(args)...);
        }
        
        ValuePair() = default;
        ValuePair( const ValuePair& ) = default;
        ValuePair( ValuePair&& ) = default;
        ValuePair& operator = ( const ValuePair& ) = default;
        ValuePair& operator = ( ValuePair&& ) = default;

        ~ValuePair() = default;
    };
    using value_vector = std::vector<ValuePair>;
    
public:
    HandleVector() = default;
    ~HandleVector() = default;
    
    HandleVector( const HandleVector &copy ) = default;
    HandleVector( HandleVector &&move ) = default;
    
    HandleVector& operator = ( const HandleVector &copy ) = default;
    HandleVector& operator = ( HandleVector &&move ) = default;

public:
    bool get( Handle handle, Value &value ) const {
        const Value *tmp = findValue(handle);
        if (tmp == nullptr) return false;
        value = *tmp;
        return true;
    }
    
    bool set( Handle handle, const Value &value ) {
        Value *tmp = find(handle);
        if (tmp == nullptr) return false;
        *tmp = value;
        return true;
    }

    Value* find( Handle handle ) {
        return const_cast<Value*>(
            findValue(handle)
        );
    }
    
    const Value* find( Handle handle ) const {
        return findValue(handle);
    }

    template< typename Func >
    Handle findIf( Func &&func ) const {
        for (const auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;

            if (func(*entry.value())) {
                return entry.handle;
            }
        }
        return Handle();
    }

    template< typename Func >
    void forEach( Func &&func ) {
        for (auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;
            func(*entry.value());
        }
    }
    
    template< typename Func >
    void forEach( Func &&func ) const {
        for (const auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;
            func(*entry.value());
        }
    }

    template< typename Func >
    void forEachWithHandle( Func &&func ) {
        for (auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;
            func(entry.handle, *entry.value());
        }
    }
    
    template< typename Func >
    void forEachWithHandle( Func &&func ) const {
        for (const auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;
            func(entry.handle, *entry.value());
        }
    }
    
    template< typename Func >
    void forEachHandle( Func &&func ) const {
        for (const auto &entry : mValues) {
            if (IsHandleFree(entry.handle)) continue;
            func(entry.handle);
        }
    }
    
    Handle create( const Value &value ) {
        return emplace(value);
    }
    
    Handle create( Value&& value ) {
        return emplace(std::move(value));
    }

    template< typename ...Args, bool Manual = ManualHandles >
    typename std::enable_if<Manual == false, Handle>::type
    emplace( Args&&... args ) {
        if (mFreeListHead == 0) {
            if (mValues.size() >= MaxValues) return Handle();

            Handle handle = CreateHandle(0, (underlaying_type)mValues.size()+1, DataValue);
            mValues.push_back( ValuePair(handle,std::forward<Args>(args)...) );
            return handle;
        }
        
        ValuePair &head = mValues[mFreeListHead-1];

        underlaying_type index = mFreeListHead;
        underlaying_type generation = GetGeneration(head.handle);
        mFreeListHead = GetIndex(head.handle);

        head.handle = CreateHandle(generation+1, index, DataValue);
        head.construct(std::forward<Args>(args)...);
        return head.handle;
    }
    
    template< typename ...Args, bool Manual = ManualHandles >
    typename std::enable_if<Manual == true>::type
    allocate( Handle handle, Args&&... args ) {
        assert(handle && IsHandleFromThis(handle));

        underlaying_type idx = GetIndex(handle);
        assert(idx > 0);
        idx -= 1;

        if (idx >= mValues.size()) {
            size_t size = idx +1;
            size *= 2;

            if (size < 1024) size = 1024;
            mValues.resize(size);
        }

        ValuePair &val = mValues[idx];
        assert(IsHandleFree(val.handle)); // handle must be free (not already allocated)

        val.handle = handle;
        val.construct(std::forward<Args>(args)...);
    }

    bool free( Handle handle ) {
        const ValuePair *tmp = nullptr;
        if (!validate(handle, tmp)) return false;

        ValuePair *data = const_cast<ValuePair*>(tmp);

        underlaying_type index = GetIndex(handle);
        underlaying_type generation = GetGeneration(data->handle);
        data->handle = CreateHandle(generation, 0, FreeDataValue);
        data->destroy();


        if (ManualHandles == false) {
            // do we have a free list?
            if (mFreeListHead == 0) { // if not create one
                mFreeListHead = index;
                mFreeListTail = index;
            }
            else {
                ValuePair &tail = mValues[mFreeListTail-1];
                tail.handle = SetIndex(tail.handle, index);
                mFreeListTail = index;
            }
        }
        
        return true;
    }

    // Free's all handles
    void clear()
    {
        underlaying_type index = 1;
        for (auto &entry : mValues) {
            index++;
            if (!IsHandleFree(entry.handle)) {
                entry.destroy();
            }
            
            entry.handle = CreateHandle(0, index, FreeDataValue);
        }

        if (ManualHandles == false) {
            mFreeListHead = 1;
            mFreeListTail = (underlaying_type)mValues.size();
            mValues.back().handle = CreateHandle(0,0,FreeDataValue);
        }
    }

    bool valid( Handle handle )const  {
        const ValuePair *data = nullptr;
        return validate(handle, data);
    }

    size_t underlying_size() {
        return mValues.size();
    }

    iterator begin() {
        return iterator(mValues.begin(), mValues.begin(), mValues.end());
    }
    iterator end() {
        return iterator(mValues.end(), mValues.begin(), mValues.end());
    }
    const_iterator begin() const {
        return const_iterator(mValues.begin(), mValues.begin(), mValues.end());
    }
    const_iterator end() const {
        return const_iterator(mValues.end(), mValues.begin(), mValues.end());
    }


public:    
    template< typename Type >
    class IteratorBase :
        public Common::IteratorAdopter<IteratorBase<Type>, Type, std::bidirectional_iterator_tag>
    {
        using base_iterator = typename std::conditional<
                                  std::is_const<Type>::value, 
                                  typename value_vector::const_iterator,
                                  typename value_vector::iterator
                              >::type;

        base_iterator mIter, mBeg, mEnd;

    public:
        IteratorBase() = default;
        IteratorBase( const IteratorBase& ) = default;
        IteratorBase& operator = ( const IteratorBase& ) = default;

        IteratorBase( base_iterator iter, base_iterator beg, base_iterator end ) :
            mIter(iter),
            mBeg(beg),
            mEnd(end)
        {}

        const Handle& handle() const {
            return mIter->handle;
        }

        bool equal( const IteratorBase &other ) const
        {
            return mIter == other.mIter;
        }

        void increment()
        {
            if (mIter == mEnd) return;

            do {
                ++mIter;
            } while (mIter != mEnd && IsHandleFree(mIter->handle));
        }

        void decrement()
        {
            if (mIter == mBeg) return;

            do {
                --mIter;
            } while (mIter != mBeg && IsHandleFree(mIter->handle));
        }
    };



private:
    bool validate( Handle handle, const ValuePair* &data ) const {
        underlaying_type dataValue = GetData(handle);
        // data missmatch, handle isn't from this container
        if (dataValue != DataValue) return false;

        underlaying_type index = GetIndex(handle);
        if (index == 0 || index > (underlaying_type)mValues.size()) {
            // invalid index in handle
            return false;
        }

        const ValuePair &tmp = mValues[index-1];
        if (tmp.handle != handle) {
            // missmatch of generation or handle is free
            return false;
        }

        data = &tmp;
        return true;
    }

    const Value* findValue( Handle handle ) const {
        const ValuePair *data = nullptr;
        if (!validate(handle, data)) return false;
        return data->value();
    }
private:
    value_vector mValues;
    underlaying_type mFreeListHead = 0,
                     mFreeListTail = 0;
};


template< typename Handle, typename Value, unsigned GenerationBits_=HANDLE_VECTOR_DEFAULT, unsigned DataBits_=HANDLE_VECTOR_DEFAULT, typename Handle::underlaying_type DataValue_=0>
using ManuelHandleVector = HandleVector<Handle, Value, GenerationBits_, DataBits_, DataValue_, true>;