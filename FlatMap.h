#pragma once

#include <vector>
#include <algorithm>

namespace Common
{
    template< typename Key, typename Value, typename Comp = std::less<Key>>
    class FlatMap {
    public:
        using value_type = std::pair<Key, Value>;

        using vector_type = std::vector<value_type>;
        using iterator = typename vector_type::iterator;
        using const_iterator = typename vector_type::const_iterator;

        struct KeyComp {
            bool operator () (const Key &lhs, const Key &rhs) const {
                return Comp()(lhs, rhs); 
            }
            bool operator () (const value_type &lhs, const Key &rhs) const {
                return Comp()(lhs.first, rhs);
            }
            bool operator () (const Key &lhs, const value_type &rhs) const {
                return Comp()(lhs, rhs.first);
            }
            bool operator () (const value_type &lhs, const value_type &rhs) const {
                return Comp()(lhs.first, rhs.first);
            }
        };

    private:
        vector_type mElements;

    public:
        FlatMap() = default;
        FlatMap( const FlatMap& ) = default;
        FlatMap( FlatMap&& ) = default;
        FlatMap( size_t reserve ) :
            mElements(reserve)
        {}
        
        ~FlatMap() = default;

        FlatMap& operator = ( const FlatMap& ) = default;
        FlatMap& operator = ( FlatMap&& ) = default;

        FlatMap& operator = ( const vector_type &elements ) {
            mElements = elements;
            std::sort(mElements.begin(), mElements.end(), KeyComp());
            return *this;
        }
        FlatMap& operator = ( vector_type &&elements ) {
            mElements = std::move(elements);
            std::sort(mElements.begin(), mElements.end(), KeyComp());
            return *this;
        }

        template< typename... Args >
        iterator emplace( Args&&... args ) {
            value_type value(std::forward<Args>(args)...);

            iterator loc = lower_bound(value.first);
            return mElements.emplace(loc, std::move(value));
        }
        
        template< typename... Args >
        iterator emplace_replace( Args&&... args ) {
            value_type value(std::forward<Args>(args)...);

            iterator loc = lower_bound(value.first);
            if (loc == end() || loc->first != value.first) {
                return mElements.emplace(loc, std::move(value));
            }
            *loc = std::move(value);
            return loc;
        }

        template< typename Key_t, typename Value_t >
        typename std::enable_if<
            std::is_same<typename std::decay<Key_t>::type, Key>::value &&
            std::is_same<typename std::decay<Value_t>::type, Value>::value,
            iterator
        >::type
        insert( Key_t &&key, Value_t &&type ) {
            return emplace(std::forward<Key_t>(key), std::forward<Value_t>(type));
        }

        template< typename Key_t, typename Value_t >
        typename std::enable_if<
            std::is_same<typename std::decay<Key_t>::type, Key>::value &&
            std::is_same<typename std::decay<Value_t>::type, Value>::value,
            iterator
        >::type
        insert_replace( Key_t &&key, Value_t &&type ) {
            return emplace_replace(std::forward<Key_t>(key), std::forward<Value_t>(type));
        }

        iterator find( const Key &key ) {
            auto iter = lower_bound(key);
            if (iter == end() || iter->first != key) {
                return end();
            }
            return iter;
        }

        const_iterator find( const Key &key ) const {
            auto iter = lower_bound(key);
            if (iter == end() || iter->first != key) {
                return end();
            }
            return iter;
        }

        size_t size() const {
            return mElements.size();
        }

        iterator begin() {
            return mElements.begin();
        }
        iterator end() {
            return mElements.end();
        }
        const_iterator begin() const {
            return mElements.begin();
        }
        const_iterator end() const {
            return mElements.end();
        }
        const_iterator cbegin() const {
            return mElements.begin();
        }
        const_iterator cend() const {
            return mElements.end();
        }

        iterator lower_bound( const Key &key ) {
            return std::lower_bound(mElements.begin(), mElements.end(), key, KeyComp{});
        }
        iterator upper_bound( const Key &key ) {
            return std::upper_bound(mElements.begin(), mElements.end(), key, KeyComp{});
        }

        const_iterator lower_bound( const Key &key ) const {
            return std::lower_bound(mElements.begin(), mElements.end(), key, KeyComp{});
        }
        const_iterator upper_bound( const Key &key ) const {
            return std::upper_bound(mElements.begin(), mElements.end(), key, KeyComp{});
        }
    };
}