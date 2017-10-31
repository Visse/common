#pragma once

#include <iterator>

namespace Common
{
    template< typename Func, typename Type=decltype(std::declval<Func>()(0))>
    class GeneratorN {
        ptrdiff_t count = 0;
        Func func;

    public:
        GeneratorN() = delete;
        GeneratorN( size_t count_, Func func_ ) :
            count(count_),
            func(func_)
        {}
        GeneratorN( const GeneratorN& ) = default;
        GeneratorN& operator = ( const GeneratorN& ) = default;

        class iterator :
            public std::iterator<std::random_access_iterator_tag, Type> 
        {
            GeneratorN *generator = nullptr;
            ptrdiff_t idx = 0;
        public:
            iterator() = default;
            ~iterator() = default;
            iterator(const iterator&) = default;
            iterator& operator = (const iterator&) = default;

            iterator( GeneratorN *gen, ptrdiff_t i) :
                generator(gen),
                idx(i)
            {}

            Type operator *() const {
                return generator->func(idx);
            }

            iterator& operator -- () {
                idx--;
                return *this;
            }

            iterator operator -- (int) {
                iterator tmp(*this);
                --tmp;
                return tmp;
            }

            iterator& operator ++ () {
                idx++;
                return *this;
            }

            iterator operator ++ (int) {
                iterator tmp(*this);
                ++tmp;
                return tmp;
            }

            iterator& operator += (ptrdiff_t n) {
                idx += n;
                return *this;
            }
            iterator& operator -= (ptrdiff_t n) {
                idx += n;
                return *this;
            }

            friend iterator operator + (const iterator &lhs, ptrdiff_t n) {
                iterator tmp(rhs);
                tmp += n;
                return tmp;
            }
            friend iterator operator + (ptrdiff_t n, const iterator &rhs) {
                iterator tmp(rhs);
                tmp += n;
                return tmp;
            }
            friend iterator operator - (const iterator &lhs, ptrdiff_t n) {
                iterator tmp(rhs);
                tmp -= n;
                return tmp;
            }
            friend iterator operator - (ptrdiff_t n, const iterator &rhs) {
                iterator tmp(rhs);
                tmp -= n;
                return tmp;
            }
            friend ptrdiff_t operator - (const iterator &lhs, const iterator &rhs) {
                return lhs.idx - rhs.idx;
            }
        
            friend bool operator < (const iterator &lhs, const iterator &rhs) {
                return lhs.idx < rhs.idx;
            }
            friend bool operator > (const iterator &lhs, const iterator &rhs) {
                return lhs.idx > rhs.idx;
            }
            friend bool operator <= (const iterator &lhs, const iterator &rhs) {
                return lhs.idx <= rhs.idx;
            }
            friend bool operator >= (const iterator &lhs, const iterator &rhs) {
                return lhs.idx >= rhs.idx;
            }

            friend bool operator == (const iterator &lhs, const iterator &rhs) {
                if (lhs.generator != rhs.generator) return false;
                if (lhs.idx != rhs.idx) return false;
                return true;
            }
            friend bool operator != (const iterator &lhs, const iterator &rhs) {
                return !(lhs == rhs);
            }
        };

        iterator begin() {
            return iterator(this, 0);
        }
        iterator end() {
            return iterator(this,count);
        }
    };

    template<typename Func >
    GeneratorN<Func> CreateGeneratorN(ptrdiff_t n, Func &&func) {
        return GeneratorN<Func>(n, std::forward<Func>(func));
    }
}