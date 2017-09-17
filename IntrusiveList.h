#pragma once


#include <type_traits>
#include <cassert>

namespace Common
{
    template<typename Type, size_t Dim=1>
    struct IntrusiveListEntry {
        using entry_type = Type;
        static const size_t dimensions = Dim;

        Type *prev[Dim] = {},
             *next[Dim] = {};

        IntrusiveListEntry() = default;

        IntrusiveListEntry( const IntrusiveListEntry& ) = delete;
        IntrusiveListEntry& operator = ( const IntrusiveListEntry& ) = delete;

        void unlinkAll() {
            for (size_t i=0; i < Dim; ++i){
                if (prev[i])
                    prev[i]->next[i] = next[i];
                if (next[i])
                    next[i]->prev[i] = prev[i];

                prev[i] = next[i] = nullptr;
            }
        }
    };

    template< typename Type, size_t DimIdx=0 >
    struct ListBuilder {
        static_assert(DimIdx < Type::dimensions, "Invalid DimIdx");

        Type *head = nullptr,
             *prev = nullptr;

        void add( Type *item ) {
            assert(item->prev[DimIdx] == nullptr);
            assert(item->next[DimIdx] == nullptr);

            if (!head) {
                head = item;
            }
            else {
                item->prev[DimIdx] = prev;
                prev->next[DimIdx] = item;
            }
            prev = item;
        }

        Type* build() {
            if (prev) {
                prev->next[DimIdx] = head;
                head->prev[DimIdx] = prev;
            }
            Type *list = head;
            head = prev = nullptr;
            return list;
        }

        bool empty() {
            return head == nullptr;
        }
    };

    template< typename Type, size_t DimIdx=0 >
    struct IntrusiveList {
        static_assert(DimIdx < Type::dimensions, "Invalid DimIdx");

        Type sentry;
        
        IntrusiveList( const IntrusiveList& ) = delete;
        IntrusiveList& operator = ( const IntrusiveList& ) = delete;

        IntrusiveList() {
            sentry.prev[DimIdx] = &sentry;
            sentry.next[DimIdx] = &sentry;
        }
        IntrusiveList( IntrusiveList &&move ) :
            IntrusiveList()
        {
            if (!move.empty()) {
                insert_back(move.release());
            }
        }
        
        IntrusiveList& operator = ( IntrusiveList &&move )
        {
            if (this == &move) return *this;
            release();
            insert_back(move.release());
            return *this;
        }

        void insert_back( Type *list )
        {
            Type *last = sentry.prev[DimIdx];
            last->next[DimIdx] = list;
            sentry.prev[DimIdx] = list->prev[DimIdx];

            list->prev[DimIdx]->next[DimIdx] = &sentry;
            list->prev[DimIdx] = last;
        }

        Type* release()
        {
            if (empty()) return nullptr;

            Type *first = sentry.next[DimIdx];
            Type *last = sentry.prev[DimIdx];

            first->prev[DimIdx] = last;
            last->next[DimIdx] = first;

            sentry.next[DimIdx] = sentry.prev[DimIdx] = &sentry;
            return first;
        }

        bool empty() const {
            return sentry.next[DimIdx] == &sentry;
        }

        bool validateList() const
        {
            const Type *end = &sentry;
            const Type *prev = &sentry;

            const Type *item = sentry.next[DimIdx];

            for (; item != end;)
            {
                if (item->prev[DimIdx] != prev) 
                    return false;
                if (item->next[DimIdx] == nullptr)
                    return false;

                prev = item;
                item = item->next[DimIdx];
            }
            if (item->prev[DimIdx] != prev) 
                return false;
            return true;
        }

        size_t size() const {
            size_t i = 0;
            const Type *item = sentry.next[DimIdx];
            const Type *end = &sentry;
            for (; item != end; item = item->next[DimIdx]) ++i;

            return i;
        }
    
        template< bool c >
        class BaseIterator :
            public std::iterator<std::bidirectional_iterator_tag, typename std::conditional<c, const Type, Type>::type>
        {
            pointer item = nullptr;

        public:
            explicit BaseIterator( pointer item_ ) :
                item(item_)
            {}

            BaseIterator() = default;
            BaseIterator( const BaseIterator& ) = default;
            BaseIterator& operator = ( const BaseIterator& ) = default;


            operator BaseIterator<true> () const {
                return BaseIterator(item);
            }

            friend bool operator == ( const BaseIterator &lhs, const BaseIterator &rhs ) {
                return lhs.item == rhs.item;
            }
            friend bool operator != ( const BaseIterator &lhs, const BaseIterator &rhs ) {
                return lhs.item != rhs.item;
            }

            reference operator * () {
                return *item;
            }
            pointer operator -> () {
                return item;
            }
            pointer get() {
                return item;
            }

            BaseIterator& operator ++ () {
                item = item->next[DimIdx];
                return *this;
            }
            BaseIterator operator ++ (int) {
                BaseIterator copy(*this);
                ++copy;
                return copy;
            }

            BaseIterator& operator -- () {
                item = item->prev[DimIdx];
                return *this;
            }
            BaseIterator operator -- (int) {
                BaseIterator copy(*this);
                --copy;
                return copy;
            }
        };
    
        using iterator = BaseIterator<false>;
        using const_iterator = BaseIterator<true>;

        iterator begin() {
            return iterator(sentry.next[DimIdx]);
        }
        iterator end() {
            return iterator(&sentry);
        }

        const_iterator cbegin() const {
            return const_iterator(sentry.next[DimIdx]);
        }
        const_iterator cend() const {
            return const_iterator(&sentry);
        }
        const_iterator begin() const {
            return cbegin();
        }
        const_iterator end() const {
            return cend();
        }
    };

}
