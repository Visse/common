#pragma once

#include "build_config.h"
#include "HandleType.h"
#include "PImplHelper.h"
#include "HandleVector.h"

// A half edge structure for storing a mesh topology
//  Read about half edge structure here:
//      - http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml
//      - https://en.wikipedia.org/wiki/Doubly_connected_edge_list
//      - https://www.openmesh.org/media/Documentations/OpenMesh-Doc-Latest/a03926.html
//      - http://kaba.hilvi.org/homepage/blog/halfedge/halfedge.htm
// This class only handles manifold surfaces (holes and boundary is supported)

namespace Common
{
    class HalfEdgeMeshBase;
    namespace internal 
    {
        struct HalfEdgeMeshBaseImpl;
        HalfEdgeMeshBaseImpl* getImpl( HalfEdgeMeshBase &mesh );
        const HalfEdgeMeshBaseImpl* getImpl( const HalfEdgeMeshBase &mesh );
    }

    // Due to the amount of iterators, they are implemented in HalfEdgeMeshIterators.h
    namespace HalfEdgeMeshIterators
    {
        class EndIterator;

        class VertexIterator;
        class HEdgeIterator;
        class EdgeIterator;
        class FaceIterator;

        class VertexVertexIterator;
        class VertexOHEdgeIterator;
        class VertexIHEdgeIterator;
        class VertexEdgeIterator;
        class VertexFaceIterator;

        class FaceVertexIterator;
        class FaceHEdgeIterator;
        class FaceEdgeIterator;
        class FaceFaceIterator;


        // special range class for use in for-range constructs
        class VertexRange;
        class HEdgeRange;
        class EdgeRange;
        class FaceRange;

        class VertexVertexRange;
        class VertexOHEdgeRange;
        class VertexIHEdgeRange;
        class VertexEdgeRange;
        class VertexFaceRange;

        class FaceVertexRange;
        class FaceHEdgeRange;
        class FaceEdgeRange;
        class FaceFaceRange;
    }

    class HalfEdgeMeshBase {
    public:
        MAKE_HANDLE(VertexHandle, uint32_t);
        MAKE_HANDLE(HEdgeHandle, uint32_t);
        MAKE_HANDLE(EdgeHandle, uint32_t);
        MAKE_HANDLE(FaceHandle, uint32_t);

        // A special iterator to mark the end
        using EndIterator = HalfEdgeMeshIterators::EndIterator;

        // Iterates over all the vertexes
        using VertexIterator = HalfEdgeMeshIterators::VertexIterator;
        // Iterates over all the half edges
        using HEdgeIterator  = HalfEdgeMeshIterators::HEdgeIterator;
        // Iterates over all the edges
        using EdgeIterator   = HalfEdgeMeshIterators::EdgeIterator;
        // Iterates over all the faces
        using FaceIterator   = HalfEdgeMeshIterators::FaceIterator;

        // Iterates over all neigboring vertexes for a given vertex
        using VertexVertexIterator = HalfEdgeMeshIterators::VertexVertexIterator;
        // Iterates over every outgoing half edge for a given vertex
        using VertexOHEdgeIterator = HalfEdgeMeshIterators::VertexOHEdgeIterator;
        // Iterates over the incoming half edges for a given vertex
        using VertexIHEdgeIterator = HalfEdgeMeshIterators::VertexIHEdgeIterator;
        // Iterates over the outgoing half edges for a given vertex
        using VertexEdgeIterator   = HalfEdgeMeshIterators::VertexEdgeIterator;
        // Iterates over all faces sharing this vertex
        using VertexFaceIterator   = HalfEdgeMeshIterators::VertexFaceIterator;

        // Iterates over the vertexes that make up the given face
        using FaceVertexIterator   = HalfEdgeMeshIterators::FaceVertexIterator;
        // Iterates over the half edges that make up the given face
        using FaceHEdgeIterator    = HalfEdgeMeshIterators::FaceHEdgeIterator;
        // Iterates over the edges that make up the given face
        using FaceEdgeIterator     = HalfEdgeMeshIterators::FaceEdgeIterator;
        // Iterates over neighboring faces for the given face
        using FaceFaceIterator     = HalfEdgeMeshIterators::FaceFaceIterator;


        using VertexRange = HalfEdgeMeshIterators::VertexRange;
        using HEdgeRange  = HalfEdgeMeshIterators::HEdgeRange;
        using EdgeRange   = HalfEdgeMeshIterators::EdgeRange; 
        using FaceRange   = HalfEdgeMeshIterators::FaceRange;

        using VertexVertexRange = HalfEdgeMeshIterators::VertexVertexRange;
        using VertexOHEdgeRange = HalfEdgeMeshIterators::VertexOHEdgeRange;
        using VertexIHEdgeRange = HalfEdgeMeshIterators::VertexIHEdgeRange;
        using VertexEdgeRange   = HalfEdgeMeshIterators::VertexEdgeRange;
        using VertexFaceRange   = HalfEdgeMeshIterators::VertexFaceRange;

        using FaceVertexRange = HalfEdgeMeshIterators::FaceVertexRange;
        using FaceHEdgeRange  = HalfEdgeMeshIterators::FaceHEdgeRange;
        using FaceEdgeRange   = HalfEdgeMeshIterators::FaceEdgeRange;
        using FaceFaceRange   = HalfEdgeMeshIterators::FaceFaceRange;

    public:
        COMMON_API HalfEdgeMeshBase();
        COMMON_API virtual ~HalfEdgeMeshBase();

        COMMON_API HalfEdgeMeshBase( const HalfEdgeMeshBase &copy );
        COMMON_API HalfEdgeMeshBase( HalfEdgeMeshBase &&move );
        
        COMMON_API HalfEdgeMeshBase& operator = ( const HalfEdgeMeshBase &copy );
        COMMON_API HalfEdgeMeshBase& operator = ( HalfEdgeMeshBase &&move );

        COMMON_API void clear();

        COMMON_API VertexHandle createVertex();
        COMMON_API EdgeHandle createEdge( VertexHandle v1, VertexHandle v2 );
        COMMON_API FaceHandle createFace( const VertexHandle *vertexes, size_t vertexCount );
        COMMON_API FaceHandle createFace( const HEdgeHandle *hedges, size_t count );

        COMMON_API HEdgeHandle findHEdge( VertexHandle v1, VertexHandle v2 ) const;
        COMMON_API EdgeHandle findEdge( VertexHandle v1, VertexHandle v2 ) const;

        COMMON_API HEdgeHandle getHEdgePair( HEdgeHandle hedge ) const;
        COMMON_API HEdgeHandle getHEdgeNext( HEdgeHandle hedge ) const;
        COMMON_API VertexHandle getHEdgeVertex( HEdgeHandle hedge ) const;
        COMMON_API EdgeHandle getHEdgeEdge( HEdgeHandle hedge ) const;
        COMMON_API FaceHandle getHEdgeFace( HEdgeHandle hedge ) const;

        COMMON_API HEdgeHandle getVertexHEdge( VertexHandle vertex ) const;
        COMMON_API HEdgeHandle getEdgeHEdge( EdgeHandle edge ) const;
        COMMON_API HEdgeHandle getFaceHEdge( FaceHandle face ) const;

        COMMON_API std::pair<VertexHandle, VertexHandle> getEdgeVertexes( EdgeHandle edge ) const;
        // The face handles may be null.
        COMMON_API std::pair<FaceHandle, FaceHandle> getEdgeFaces( EdgeHandle edge ) const;

        // Tests if the face is on the border,
        //  A face is on the border if its has edges without neighbors
        COMMON_API bool isBorderFace( FaceHandle face ) const;

        // Iterator functions
        COMMON_API VertexRange vertexes() const;
        COMMON_API VertexIterator vertexesBegin() const;
        COMMON_API VertexIterator vertexesEnd() const;

        COMMON_API HEdgeRange hedges() const;
        COMMON_API HEdgeIterator hedgesBegin() const;
        COMMON_API HEdgeIterator hedgesEnd() const;

        COMMON_API EdgeRange edges() const;
        COMMON_API EdgeIterator edgesBegin() const;
        COMMON_API EdgeIterator edgesEnd() const;

        COMMON_API FaceRange faces() const;
        COMMON_API FaceIterator facesBegin() const;
        COMMON_API FaceIterator facesEnd() const;

        COMMON_API VertexVertexRange vertexVertexes( VertexHandle vertex ) const;
        COMMON_API VertexVertexIterator vertexVertexesBegin( VertexHandle vertex ) const;
        COMMON_API VertexVertexIterator vertexVertexesEnd( VertexHandle vertex ) const;

        COMMON_API VertexOHEdgeRange vertexOHEdges( VertexHandle vertex ) const;
        COMMON_API VertexOHEdgeIterator vertexOHEdgesBegin( VertexHandle vertex ) const;
        COMMON_API VertexOHEdgeIterator vertexOHEdgesEnd( VertexHandle vertex ) const;

        COMMON_API VertexIHEdgeRange vertexIHEdges( VertexHandle vertex ) const;
        COMMON_API VertexIHEdgeIterator vertexIHEdgesBegin( VertexHandle vertex ) const;
        COMMON_API VertexIHEdgeIterator vertexIHEdgesEnd( VertexHandle vertex ) const;

        COMMON_API VertexEdgeRange vertexEdges( VertexHandle vertex ) const;
        COMMON_API VertexEdgeIterator vertexEdgesBegin( VertexHandle vertex ) const;
        COMMON_API VertexEdgeIterator vertexEdgesEnd( VertexHandle vertex ) const;

        COMMON_API VertexFaceRange vertexFaces( VertexHandle vertex ) const;
        COMMON_API VertexFaceIterator vertexFacesBegin( VertexHandle vertex ) const;
        COMMON_API VertexFaceIterator vertexFacesEnd( VertexHandle vertex ) const;

        COMMON_API FaceVertexRange faceVertexes( FaceHandle face ) const;
        COMMON_API FaceVertexIterator faceVertexesBegin( FaceHandle face ) const;
        COMMON_API FaceVertexIterator faceVertexesEnd( FaceHandle face ) const;

        COMMON_API FaceHEdgeRange faceHEdges( FaceHandle face ) const;
        COMMON_API FaceHEdgeIterator faceHEdgesBegin( FaceHandle face ) const;
        COMMON_API FaceHEdgeIterator faceHEdgesEnd( FaceHandle face ) const;

        COMMON_API FaceEdgeRange faceEdges( FaceHandle face ) const;
        COMMON_API FaceEdgeIterator faceEdgesBegin( FaceHandle face ) const;
        COMMON_API FaceEdgeIterator faceEdgesEnd( FaceHandle face ) const;

        COMMON_API FaceFaceRange faceFaces( FaceHandle face ) const;
        COMMON_API FaceFaceIterator faceFacesBegin( FaceHandle face ) const;
        COMMON_API FaceFaceIterator faceFacesEnd( FaceHandle face ) const;

        // Debuging functions
#if COMMON_DEBUG_LEVEL > 0
        // Verify that the internal state is correct (all the expected invariants are true)
        COMMON_API void verifyInvariants() const;
        // Dump the internal state to stdout
        COMMON_API void dumpInternalState() const;
        // Dump the internal state as a graph (in dot format) to stdout
        COMMON_API void dumpDotGraph() const;
#endif

    protected:
        COMMON_API virtual void onVertexCreated( VertexHandle handle ) {}
        COMMON_API virtual void onHEdgeCreated( HEdgeHandle handle ) {}
        COMMON_API virtual void onEdgeCreated( EdgeHandle handle ) {}
        COMMON_API virtual void onFaceCreated( FaceHandle handle ) {}
         
        COMMON_API virtual void onVertexDestroyed( VertexHandle handle ) {}
        COMMON_API virtual void onHEdgeDestroyed( HEdgeHandle handle ){}
        COMMON_API virtual void onEdgeDestroyed( EdgeHandle handle ) {}
        COMMON_API virtual void onFaceDestroyed( FaceHandle handle ) {}

        COMMON_API virtual void onClear() {}

    private:
        friend struct internal::HalfEdgeMeshBaseImpl;

        friend  internal::HalfEdgeMeshBaseImpl* internal::getImpl( HalfEdgeMeshBase& );
        friend  const internal::HalfEdgeMeshBaseImpl* internal::getImpl( const HalfEdgeMeshBase& );

        PImplHelper<internal::HalfEdgeMeshBaseImpl, 144> mImpl;
    };

    template< typename VertexData, typename HEdgeData,  typename EdgeData, typename FaceData >
    class HalfEdgeMesh :
        public HalfEdgeMeshBase
    {
    public:
        using HalfEdgeMeshBase::HalfEdgeMeshBase;



#define _CREATE_DATA_METHODS(Handle, Type, member)          \
        void setData( Handle handle, const Type &data ) {   \
            member.set(handle, data);                       \
        }                                                   \
        bool getData( Handle handle, Type &data ) const {   \
            return member.get(handle,data);                 \
        }                                                   \
        Type* findData( Handle handle ) {                   \
            return member.find(handle);                     \
        }                                                   \
        const Type* findData( Handle handle ) const {       \
            return member.find(handle);                     \
        }
        _CREATE_DATA_METHODS(VertexHandle, VertexData, mVertexes);
        _CREATE_DATA_METHODS(HEdgeHandle, HEdgeData, mHEdges);
        _CREATE_DATA_METHODS(EdgeHandle, EdgeData, mEdges);
        _CREATE_DATA_METHODS(FaceHandle, FaceData, mFaces);
#undef _CREATE_DATA_METHODS


    protected:
        virtual void onVertexCreated( VertexHandle handle ) override {
            mVertexes.allocate(handle);
        }
        virtual void onHEdgeCreated( HEdgeHandle handle ) override {
            mHEdges.allocate(handle);
        }
        virtual void onEdgeCreated( EdgeHandle handle ) override {
            mEdges.allocate(handle);
        }
        virtual void onFaceCreated( FaceHandle handle ) override {
            mFaces.allocate(handle);
        }
        
        virtual void onVertexDestroyed( VertexHandle handle ) override {
            mVertexes.free(handle);
        }
        virtual void onHEdgeDestroyed( HEdgeHandle handle )override {
            mHEdges.free(handle);
        }
        virtual void onEdgeDestroyed( EdgeHandle handle ) override {
            mEdges.free(handle);
        }
        virtual void onFaceDestroyed( FaceHandle handle ) override {
            mFaces.free(handle);
        }

        virtual void onClear() override {
            mVertexes.clear();
            mHEdges.clear();
            mEdges.clear();
            mFaces.clear();
        }

    private:
        ManuelHandleVector<VertexHandle, VertexData> mVertexes;
        ManuelHandleVector<HEdgeHandle, HEdgeData> mHEdges;
        ManuelHandleVector<EdgeHandle, EdgeData> mEdges;
        ManuelHandleVector<FaceHandle, FaceData> mFaces;
    };
}
