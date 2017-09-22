#pragma once

#include "build_config.h"
#include "PImplHelper.h"
#include "Archive.h"
#include "FromString.h"
#include "StringId.h"

#include <string>
#include <iterator>

namespace Common
{
    class YamlNode {
    public:
        static COMMON_API YamlNode LoadFile( const char *filename );
        static COMMON_API YamlNode LoadFile( Archive &archive, const char *filename );
        static COMMON_API YamlNode LoadString( const char *str, size_t len );

    public:
        enum class Type {
            Null,
            Scalar,
            Sequence,
            Map
        };

        struct Mark {
            int line=-1, col=-1;
        };

    public:
        COMMON_API YamlNode();
        COMMON_API ~YamlNode();

        COMMON_API YamlNode( const YamlNode& );
        COMMON_API YamlNode& operator = ( const YamlNode& );

        COMMON_API Type type() const;

        COMMON_API size_t size() const;

        COMMON_API YamlNode operator [] (int i) const;
        COMMON_API YamlNode operator [] (const char *str) const;
        
        COMMON_API std::string scalar() const;
        COMMON_API const char* c_str( size_t &len ) const;
        COMMON_API StringId scalarAsStringId() const;

        class const_iterator;
        struct IteratorElement;
        
        COMMON_API const_iterator begin() const;
        COMMON_API const_iterator end() const;

        COMMON_API Mark mark() const;

        template< typename Type >
        bool as( Type &value ) const {
            if (!isScalar()) return false;
            return FromString(_str(), _len(), value);
        }

        explicit operator bool () const {
            return type() != Type::Null;
        }
        bool operator ! () const {
            return type() == Type::Null;
        }

        bool isScalar() const {
            return type() == Type::Scalar;
        }
        bool isSequence() const {
            return type() == Type::Sequence;
        }
        bool isMap() const {
            return type() == Type::Map;
        }
        COMMON_API friend bool operator == ( const YamlNode &lhs, const YamlNode &rhs );

        friend bool operator != ( const YamlNode &lhs, const YamlNode &rhs ) {
            return !operator ==(lhs, rhs);
        }

    private:
        COMMON_API const char* _str() const;
        COMMON_API size_t _len() const;

    private:
        struct Impl;
        PImplHelper<Impl, 24> mImpl;
    };

    struct YamlNode::IteratorElement :
        public YamlNode,
        public std::pair<YamlNode, YamlNode>
    {
        IteratorElement() = default;
        IteratorElement( const IteratorElement& ) = default;
        IteratorElement& operator = ( const IteratorElement& ) = default;

        IteratorElement( const YamlNode &node ) :
            YamlNode(node)
        {}
        IteratorElement( const std::pair<YamlNode, YamlNode> &pair ) :
            std::pair<YamlNode,YamlNode>(pair)
        {}
    };


    class YamlNode::const_iterator :
        public std::iterator<std::random_access_iterator_tag, YamlNode::IteratorElement>
    {
        friend class YamlNode;
        COMMON_API void advance( ptrdiff_t n );
        COMMON_API IteratorElement current() const;

    public:
        COMMON_API const_iterator();
        COMMON_API const_iterator( const const_iterator& );
        COMMON_API ~const_iterator();
        COMMON_API const_iterator& operator = ( const const_iterator& );

        const_iterator& operator ++ () {
            advance(1);
            return *this;
        }
        const_iterator operator ++ (int) {
            const_iterator copy(*this);
            advance(1);
            return copy;
        }
        const_iterator& operator -- () {
            advance(-1);
            return *this;
        }
        const_iterator operator -- (int) {
            const_iterator copy(*this);
            advance(-1);
            return copy;
        }

        COMMON_API friend bool operator == ( const const_iterator &lhs, const const_iterator &rhs );
        friend bool operator != ( const const_iterator &lhs, const const_iterator &rhs ) {
            return !operator ==(lhs, rhs);
        }

        IteratorElement operator * () const {
            return current();
        }

        friend const_iterator operator + ( const const_iterator &iter, ptrdiff_t n ) {
            const_iterator copy(iter);
            copy.advance(n);
            return copy;
        }
        friend const_iterator operator + ( ptrdiff_t n, const const_iterator &iter ) {
            const_iterator copy(iter);
            copy.advance(n);
            return copy;
        }
        
        friend const_iterator operator - ( const const_iterator &iter, ptrdiff_t n ) {
            const_iterator copy(iter);
            copy.advance(-n);
            return copy;
        }
        friend const_iterator operator - ( ptrdiff_t n, const const_iterator &iter ) {
            const_iterator copy(iter);
            copy.advance(-n);
            return copy;
        }
        
        const_iterator& operator += ( ptrdiff_t n ) {
            advance(n);
            return *this;
        }
        const_iterator& operator -= ( ptrdiff_t n ) {
            advance(-n);
            return *this;
        }

    private:
        struct Impl;
        PImplHelper<Impl, 40> mImpl;
    };
}