#pragma once

#include <iterator>


namespace Common
{
    namespace internal
    {
        template< typename Adopter, typename Type, typename Tag >
        struct Iterator;

        template< typename Adopter >
        auto _derefernce_must_return_a_reference_for_operator_pointer_to_work( Adopter *iter ) ->
        typename std::enable_if<
            std::is_reference<decltype(iter->dereference())>::value,
            decltype(&iter->dereference())
        >::type
        {
            return &iter->dereference();
        }

        template<typename Adopter, typename Type>
        struct Iterator<Adopter, Type, std::forward_iterator_tag>
        {
            typedef ptrdiff_t difference_type;
            typedef Type value_type;
            typedef value_type* pointer;
            typedef value_type& reference;
            typedef std::forward_iterator_tag iterator_category;

            Adopter* adopter() {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::forward_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<Adopter*>(this);
            }

            const Adopter* adopter() const {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::forward_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<const Adopter*>(this);
            }
            
            friend bool operator == (const Adopter &lhs, const Adopter &rhs ) 
            {
                return lhs.adopter()->equal(*rhs.adopter());
            }
            friend bool operator != (const Adopter &lhs, const Adopter &rhs ) 
            {
                return !lhs.adopter()->equal(*rhs.adopter());
            }
            
            decltype(auto) operator* () {
                return adopter()->dereference();
            }

            decltype(auto) operator -> () {
                return _derefernce_must_return_a_reference_for_operator_pointer_to_work(adopter());
            }

            Adopter& operator ++ () {
                adopter()->increment();
                return *adopter();
            }
            Adopter operator ++ (int) {
                Adopter copy(*adopter());
                adopter()->increment();
                return copy;
            }
            
        };
        
        template<typename Adopter, typename Type>
        struct Iterator<Adopter, Type, std::bidirectional_iterator_tag> :
            public Iterator<Adopter, Type, std::forward_iterator_tag> 
        {
            typedef ptrdiff_t difference_type;
            typedef Type value_type;
            typedef value_type* pointer;
            typedef value_type& reference;
            typedef std::bidirectional_iterator_tag iterator_category;

            Adopter* adopter() {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::bidirectional_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<Adopter*>(this);
            }

            const Adopter* adopter() const {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::bidirectional_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<const Adopter*>(this);
            }

            Adopter& operator -- () {
                adopter()->decrement();
                return *adopter();
            }
            Adopter operator -- (int) {
                Adopter copy(*adopter());
                adopter()->decrement();
                return copy;
            }
        };

        template<typename Adopter, typename Type>
        struct Iterator<Adopter, Type, std::random_access_iterator_tag> :
            public Iterator<Adopter, Type, std::bidirectional_iterator_tag>
        {
            typedef ptrdiff_t difference_type;
            typedef Type value_type;
            typedef value_type* pointer;
            typedef value_type& reference;
            typedef std::random_access_iterator_tag iterator_category;
            

            Adopter* adopter() {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::random_access_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<Adopter*>(this);
            }

            const Adopter* adopter() const {
                static_assert(std::is_base_of<Iterator<Adopter, Type, std::random_access_iterator_tag>, Adopter>::value, "Adopter is not correcly derived!");
                return static_cast<const Adopter*>(this);
            }

            friend Adopter operator + ( Adopter lhs, difference_type n ) {
                lhs.adopter()->advance(n);
                return *lhs.adopter();
            }
            friend Adopter operator + ( difference_type n, Adopter rhs ) {
                rhs.adopter()->advance(n);
                return *rhs.adopter();
            }
            friend Adopter operator - ( Adopter lhs, difference_type n ) {
                lhs.adopter()->advance(-n);
                return *lhs.adopter();
            }
            friend difference_type operator - ( const Adopter &lhs, const Adopter &rhs ) {
                return rhs.adopter()->distance(*lhs.adopter());
            }

            friend bool operator < ( const Adopter &lhs, const Adopter &rhs )
            {
                return (lhs - rhs) < difference_type(0);
            }

            friend bool operator > ( const Adopter &lhs, const Adopter &rhs )
            {
                return (lhs - rhs) > difference_type(0);
            }

            friend bool operator <= ( const Adopter &lhs, const Adopter &rhs )
            {
                return (lhs - rhs) <= difference_type(0);
            }

            friend bool operator >= ( const Adopter &lhs, const Adopter &rhs )
            {
                return (lhs - rhs) >= difference_type(0);
            }

            Adopter& operator += ( difference_type n ) {
                adopter()->advance(n);
                return *adopter();
            }

            Adopter& operator -= ( difference_type n ) {
                adopter()->advance(-n);
                return *adopter();
            }

            reference operator [] ( difference_type n ) const {
                Adopter copy(*adopter());
                copy.advance(n);
                return *copy;
            }
        };
    }

    /* Adopter for easier creation of iterators.
     * Usage:
            struct Iterator :
                public IteratorAdopter<Iterator, Type, Tag> 
            {
                ...
            };


        The following functions have to be implemented for tag
            std::forward_iterator_tag
               - equal
               - increment
            std::bidirectional_iterator_tag
               - decrement
            std::random_access_iterator_tag
                - advance
                - distance
                
     */
    template< typename Adopter, typename Type, typename Tag >
    class IteratorAdopter :
        public internal::Iterator<Adopter, Type, Tag>
    {
        IteratorAdopter() = default;
        friend Adopter;
    };
}