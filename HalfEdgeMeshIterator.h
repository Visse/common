#pragma once

#include "build_config.h"
#include "PImplHelper.h"
#include "IteratorAdopter.h"
#include "HalfEdgeMesh.h"

// A half edge structure for storing a mesh topology
//  Read about half edge structure here:
//      - http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
//      - https://en.wikipedia.org/wiki/Doubly_connected_edge_list
//      - https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03926.html
//      - http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
// This class only handles manifold surfaces (holes and boundary is supported)

namespace Common
{
    namespace HalfEdgeMeshIterators
    {
        using VertexHandle = HalfEdgeMeshBase::VertexHandle;
        using HEdgeHandle = HalfEdgeMeshBase::HEdgeHandle;
        using EdgeHandle = HalfEdgeMeshBase::EdgeHandle;
        using FaceHandle = HalfEdgeMeshBase::FaceHandle;

        class EndIterator {};

        class VertexIterator :
            public IteratorAdopter<VertexIterator, VertexHandle, std::bidirectional_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 24> mImpl;
        public:
            COMMON_API explicit VertexIterator( const HalfEdgeMeshBase &mesh );
            COMMON_API VertexIterator( const HalfEdgeMeshBase &mesh, EndIterator end );
            COMMON_API ~VertexIterator();

            COMMON_API VertexIterator( const VertexIterator &copy );
            COMMON_API VertexIterator& operator = ( const VertexIterator &copy );

            COMMON_API bool equal( const VertexIterator &other ) const;
            COMMON_API VertexHandle dereference() const;

            COMMON_API void increment();
            COMMON_API void decrement();
        };

        class HEdgeIterator :
            public IteratorAdopter<HEdgeIterator, HEdgeHandle, std::bidirectional_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 24> mImpl;
        public:
            COMMON_API HEdgeIterator( const HalfEdgeMeshBase &mesh );
            COMMON_API HEdgeIterator( const HalfEdgeMeshBase &mesh, EndIterator end );
            COMMON_API ~HEdgeIterator();

            COMMON_API HEdgeIterator( const HEdgeIterator &copy );
            COMMON_API HEdgeIterator& operator = ( const HEdgeIterator &copy );

            COMMON_API bool equal( const HEdgeIterator &other ) const;
            COMMON_API HEdgeHandle dereference() const;

            COMMON_API void increment();
            COMMON_API void decrement();
        };

        class EdgeIterator :
            public IteratorAdopter<EdgeIterator, EdgeHandle, std::bidirectional_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 24> mImpl;
        public:
            COMMON_API EdgeIterator( const HalfEdgeMeshBase &mesh );
            COMMON_API EdgeIterator( const HalfEdgeMeshBase &mesh, EndIterator end );
            COMMON_API ~EdgeIterator();

            COMMON_API EdgeIterator( const EdgeIterator &copy );
            COMMON_API EdgeIterator& operator = ( const EdgeIterator &copy );

            COMMON_API bool equal( const EdgeIterator &other ) const;
            COMMON_API EdgeHandle dereference() const;

            COMMON_API void increment();
            COMMON_API void decrement();
        };

        class FaceIterator :
            public IteratorAdopter<FaceIterator, FaceHandle, std::bidirectional_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 24> mImpl;
        public:
            COMMON_API FaceIterator( const HalfEdgeMeshBase &mesh );
            COMMON_API FaceIterator( const HalfEdgeMeshBase &mesh, EndIterator end );
            COMMON_API ~FaceIterator();

            COMMON_API FaceIterator( const FaceIterator &copy );
            COMMON_API FaceIterator& operator = ( const FaceIterator &copy );

            COMMON_API bool equal( const FaceIterator &other ) const;
            COMMON_API FaceHandle dereference() const;

            COMMON_API void increment();
            COMMON_API void decrement();
        };
        
        class VertexVertexIterator :
            public IteratorAdopter<VertexVertexIterator, VertexHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API VertexVertexIterator( const HalfEdgeMeshBase &mesh, VertexHandle vertex );
            COMMON_API VertexVertexIterator( EndIterator end );
            COMMON_API ~VertexVertexIterator();

            COMMON_API VertexVertexIterator( const VertexVertexIterator &copy );
            COMMON_API VertexVertexIterator& operator = ( const VertexVertexIterator &copy );

            COMMON_API bool equal( const VertexVertexIterator &other ) const;
            COMMON_API HalfEdgeMeshBase::VertexHandle dereference() const;

            COMMON_API void increment();
        };

        class VertexOHEdgeIterator  :
            public IteratorAdopter<VertexOHEdgeIterator, HEdgeHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API VertexOHEdgeIterator( const HalfEdgeMeshBase &mesh, VertexHandle vertex );
            COMMON_API VertexOHEdgeIterator( EndIterator end );
            COMMON_API ~VertexOHEdgeIterator();

            COMMON_API VertexOHEdgeIterator( const VertexOHEdgeIterator &copy );
            COMMON_API VertexOHEdgeIterator& operator = ( const VertexOHEdgeIterator &copy );

            COMMON_API bool equal( const VertexOHEdgeIterator &other ) const;
            COMMON_API HEdgeHandle dereference() const;

            COMMON_API void increment();
        };

        class VertexIHEdgeIterator :
            public IteratorAdopter<VertexIHEdgeIterator, HEdgeHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API VertexIHEdgeIterator( const HalfEdgeMeshBase &mesh, VertexHandle vertex );
            COMMON_API VertexIHEdgeIterator( EndIterator end );
            COMMON_API ~VertexIHEdgeIterator();

            COMMON_API VertexIHEdgeIterator( const VertexIHEdgeIterator &copy );
            COMMON_API VertexIHEdgeIterator& operator = ( const VertexIHEdgeIterator &copy );

            COMMON_API bool equal( const VertexIHEdgeIterator &other ) const;
            COMMON_API HalfEdgeMeshBase::HEdgeHandle dereference() const;

            COMMON_API void increment();
        };

        class VertexEdgeIterator :
            public IteratorAdopter<VertexEdgeIterator, EdgeHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API VertexEdgeIterator( const HalfEdgeMeshBase &mesh, VertexHandle vertex );
            COMMON_API VertexEdgeIterator( EndIterator end );
            COMMON_API ~VertexEdgeIterator();

            COMMON_API VertexEdgeIterator( const VertexEdgeIterator &copy );
            COMMON_API VertexEdgeIterator& operator = ( const VertexEdgeIterator &copy );

            COMMON_API bool equal( const VertexEdgeIterator &other ) const;
            COMMON_API EdgeHandle dereference() const;

            COMMON_API void increment();
        };

        class FaceVertexIterator :
            public IteratorAdopter<FaceVertexIterator, VertexHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API FaceVertexIterator( const HalfEdgeMeshBase &mesh, FaceHandle face );
            COMMON_API FaceVertexIterator( EndIterator end );
            COMMON_API ~FaceVertexIterator();

            COMMON_API FaceVertexIterator( const FaceVertexIterator &copy );
            COMMON_API FaceVertexIterator& operator = ( const FaceVertexIterator &copy );

            COMMON_API bool equal( const FaceVertexIterator &other ) const;
            COMMON_API VertexHandle dereference() const;

            COMMON_API void increment();
        };

        class FaceHEdgeIterator :
            public IteratorAdopter<FaceHEdgeIterator, HEdgeHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API FaceHEdgeIterator( const HalfEdgeMeshBase &mesh, FaceHandle face );
            COMMON_API FaceHEdgeIterator( EndIterator end );
            COMMON_API ~FaceHEdgeIterator();

            COMMON_API FaceHEdgeIterator( const FaceHEdgeIterator &copy );
            COMMON_API FaceHEdgeIterator& operator = ( const FaceHEdgeIterator &copy );

            COMMON_API bool equal( const FaceHEdgeIterator &other ) const;
            COMMON_API HEdgeHandle dereference() const;

            COMMON_API void increment();
        };

        class FaceEdgeIterator :
            public IteratorAdopter<FaceEdgeIterator, EdgeHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API FaceEdgeIterator( const HalfEdgeMeshBase &mesh, FaceHandle face );
            COMMON_API FaceEdgeIterator( EndIterator end );
            COMMON_API ~FaceEdgeIterator();

            COMMON_API FaceEdgeIterator( const FaceEdgeIterator &copy );
            COMMON_API FaceEdgeIterator& operator = ( const FaceEdgeIterator &copy );

            COMMON_API bool equal( const FaceEdgeIterator &other ) const;
            COMMON_API EdgeHandle dereference() const;

            COMMON_API void increment();
        };

        class FaceFaceIterator :
            public IteratorAdopter<FaceFaceIterator, FaceHandle, std::forward_iterator_tag>
        {
            struct Impl;
            PImplHelper<Impl, 40> mImpl;
        public:
            COMMON_API FaceFaceIterator( const HalfEdgeMeshBase &mesh, FaceHandle face );
            COMMON_API FaceFaceIterator( EndIterator end );
            COMMON_API ~FaceFaceIterator();

            COMMON_API FaceFaceIterator( const FaceFaceIterator &copy );
            COMMON_API FaceFaceIterator& operator = ( const FaceFaceIterator &copy );

            COMMON_API bool equal( const FaceFaceIterator &other ) const;
            COMMON_API FaceHandle dereference() const;

            COMMON_API void increment();
        };
    

#define _IMPLEMENT_RANGE(Type, Iter)                    \
        class Type {                                    \
            Iter mBeg, mEnd;                            \
        public:                                         \
            Type() = default;                           \
            Type( const Type& ) = default;              \
            Type& operator = ( const Type& ) = default; \
            Type( Iter beg, Iter end ) :                \
                mBeg(beg), mEnd(end)                    \
            {}                                          \
            Iter begin() const {                        \
                return mBeg;                            \
            }                                           \
            Iter end() const {                          \
                return mEnd;                            \
            }                                           \
        };

        _IMPLEMENT_RANGE(VertexRange, VertexIterator);
        _IMPLEMENT_RANGE(HEdgeRange, HEdgeIterator);
        _IMPLEMENT_RANGE(EdgeRange, EdgeIterator);
        _IMPLEMENT_RANGE(FaceRange, FaceIterator);

        
        _IMPLEMENT_RANGE(VertexVertexRange, VertexVertexIterator);
        _IMPLEMENT_RANGE(VertexOHEdgeRange, VertexOHEdgeIterator);
        _IMPLEMENT_RANGE(VertexIHEdgeRange, VertexIHEdgeIterator);
        _IMPLEMENT_RANGE(VertexEdgeRange, VertexEdgeIterator);

        
        _IMPLEMENT_RANGE(FaceVertexRange, FaceVertexIterator);
        _IMPLEMENT_RANGE(FaceHEdgeRange, FaceHEdgeIterator);
        _IMPLEMENT_RANGE(FaceEdgeRange, FaceEdgeIterator);
        _IMPLEMENT_RANGE(FaceFaceRange, FaceFaceIterator);

#undef _IMPLEMENT_RANGE
    }
}