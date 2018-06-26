#include "HalfEdgeMesh.impl.h"
#include "HalfEdgeMeshIterator.h"

#include "ErrorUtils.h"

namespace Common
{
    namespace internal 
    {
        static const int MAX_FACE_VERTEXES = 16;

        HalfEdgeMeshBaseImpl* getImpl( HalfEdgeMeshBase &mesh )
        {
            return mesh.mImpl;
        }
        
        const HalfEdgeMeshBaseImpl* getImpl( const HalfEdgeMeshBase &mesh )
        {
            return mesh.mImpl;
        }

        void init( Impl *impl, HalfEdgeMeshBase *this_ )
        {
            impl->this_ = this_;
        }

        void destroy( Impl *impl )
        {}

        void clear( Impl *impl )
        {
            impl->vertexes.clear();
            impl->hedges.clear();
            impl->edges.clear();
            impl->faces.clear();

            impl->vertexesLock++;
            impl->hedgesLock++;
            impl->edgesLock++;
            impl->facesLock++;
        }

        VertexHandle createVertex( Impl *impl )
        {
            VertexHandle handle = allocateVertex(impl);
            if (handle) {
                impl->onVertexCreated(handle);
            }
            return handle;
        }
        
        CHEdgePtr findFreeHEdge( const Impl *impl, CHEdgePtr hedge )
        {
            if (!hedge) return CHEdgePtr();

            const HEdgeHandle root = hedge;

            while (true) {
                if (!hedge->face) return hedge;

                CHEdgePtr next = hedge.pairNext();
                if (next == root) {
                    return CHEdgePtr();
                }

                hedge = next;
            }
        }
        
        HEdgePtr findFreeHEdge( Impl *impl, HEdgePtr hedge )
        {
            return asNonConst(
                findFreeHEdge(impl, CHEdgePtr(hedge))
            );
        }

        HEdgePtr findFreeHEdge( Impl *impl, HEdgePtr before, HEdgePtr after )
        {
#if COMMON_DEBUG_LEVEL > 0
            FATAL_ASSERT(after->vertex == before->vertex);
#endif

            do {
                after = after.nextPair();
                if (after == before) break;
                if (!after->face) return after;
            } while (true);

            return HEdgePtr();
        }

        bool isFree( Impl *impl, VertexPtr vertex ) 
        {
            if (!vertex) return false;

            if (!vertex->hedge) return true;

            return !!findFreeHEdge(impl, vertex.hedge());
        }

        CHEdgePtr findHEdge( const Impl *impl, CVertexPtr v1, CVertexPtr v2 )
        {
            CHEdgePtr hedge = v1.hedge();
            if (!hedge) return CHEdgePtr();

            const HEdgeHandle root = hedge;

            do {
                if (hedge->vertex == v2) {
                    return hedge;
                }

                hedge = hedge.pairNext();
            } while (hedge != root);

            return CHEdgePtr();
        }

        HEdgePtr findHEdge( Impl *impl, VertexPtr v1, VertexPtr v2 ) {
            return asNonConst(
                findHEdge(impl, CVertexPtr(v1), CVertexPtr(v2))
            );
        }

        CEdgePtr findEdge( const Impl *impl, CVertexPtr v1, CVertexPtr v2 )
        {
            CHEdgePtr hedge = findHEdge(impl, v1, v2);
            return hedge.edge();
        }
        
        EdgePtr findEdge( Impl *impl, VertexPtr v1, VertexPtr v2 )
        {
            return asNonConst(
                findEdge(impl, CVertexPtr(v1),  CVertexPtr(v2))
            );
        }

        EdgePtr createEdge( Impl *impl, VertexPtr v1, VertexPtr v2 )
        {
            if (v1 == v2) return EdgePtr();

            if (!(v1 && v2)) {
                return EdgePtr();
            }

            if (EdgePtr existing = findEdge(impl, v1, v2)) {
                return existing;
            }

            HEdgePtr v1_insertion_point,
                     v2_insertion_point;

            v1_insertion_point = findFreeHEdge(impl, v1.hedge());
            v2_insertion_point = findFreeHEdge(impl, v2.hedge());

            if (v1->hedge && !v1_insertion_point) {
                return EdgePtr();
            }
            if (v2->hedge && !v2_insertion_point) {
                return EdgePtr();
            }

            EdgePtr edge = allocateEdge(impl);

            if (!edge) {
                return EdgePtr();
            }

            HEdgePtr h1 = allocateHEdge(impl),
                     h2 = allocateHEdge(impl);

        
            if (!(h1 && h2)) {
                freeEdge(impl, edge);
                freeHEdge(impl, h1);
                freeHEdge(impl, h2);
                return EdgePtr();
            }

            edge->hedge = h1;

            {
                h1->edge = edge;
                h1->pair = h2;
                h1->next = h2;
                h1->prev = h2;
                h1->vertex = v2;
            }
            {
                h2->edge = edge;
                h2->pair = h1;
                h2->next = h1;
                h2->prev = h1;
                h2->vertex = v1;
            }

            if (!v1->hedge) {
                v1->hedge = h1;
            }
            else {
                HEdgePtr e1 = v1_insertion_point;
                HEdgePtr e2 = e1.prev();
            
                e2->next = h1;
                h1->prev = e2;

                h2->next = e1;
                e1->prev = h2;
            }

            if (!v2->hedge) {
                v2->hedge = h2;
            }
            else {
                HEdgePtr e1 = v2_insertion_point;
                HEdgePtr e2 = e1.prev();

                h1->next = e1;
                e1->prev = h1;

                e2->next = h2;
                h2->prev = e2;
            }

            impl->onHEdgeCreated(h1);
            impl->onHEdgeCreated(h2);
            impl->onEdgeCreated(edge);

            return edge;
        }
    
        bool makeAdjacent( Impl *impl, HEdgePtr in, HEdgePtr out )
        {
            if (in->next == out) {
                return true;
            }

            HEdgePtr a = in.next(),
                     b = out.prev();


            HEdgePtr c = findFreeHEdge(impl, in, out.pair());
            if (!c) return false;

            HEdgePtr d = c.next();


            in->next = out;
            out->prev = in;

            c->next = a;
            a->prev = c;

            b->next = d;
            d->prev = b;

            return true;
        }

        FacePtr createFace( Impl *impl, HEdgePtr *edges, size_t count )
        {
            // This is a internal function, so we asume that the edges we got are correct
            // but to make sure we assert that if debugging is enabled
#if COMMON_DEBUG_LEVEL > 0
            assert (count >= 3);
            for (size_t i=0; i < count; ++i) {
                FATAL_ASSERT(edges[i].valid());
                FATAL_ASSERT(
                    edges[i]->vertex == edges[(i+1)%count].pair()->vertex,
                    "The edges must form a loop"
                );
                FATAL_ASSERT(!edges[i]->face);
            }
#endif

            for (size_t i=0; i < count; ++i) {
                if (!makeAdjacent(impl, edges[i], edges[(i+1)%count])) {
                    LOG_WARNING("Failed to create face: couldn't make edges adjacent!");
                    return FacePtr();
                }
            }


            FacePtr face = allocateFace(impl);
            for (size_t i=0; i < count; ++i) {
                edges[i]->face = face;
            }
            face->hedge = edges[0];

            impl->onFaceCreated(face);

            return face;
        }

        FacePtr createFace( Impl *impl, const HEdgeHandle *handles, size_t count )
        {
            if (count < 3) {
                LOG_WARNING("Failed to create face: to few edges (%zu), a face must have atleast 3 edges", count);
                return FacePtr();
            }
            if (count >= MAX_FACE_VERTEXES) {
                LOG_WARNING("Failed to create face: to many edges (%zu), increase MAX_FACE_VERTEXES (%zu)", count, MAX_FACE_VERTEXES);
                return FacePtr();
            }

            HEdgePtr hedges[MAX_FACE_VERTEXES];
            for (size_t i=0; i < count; ++i) {
                hedges[i] = HEdgePtr(impl, handles[i]);
                if (!hedges[i]) {
                    LOG_WARNING("Failed to create face: invalid edges!");
                    return FacePtr();
                }
                if (hedges[i]->face) {
                    LOG_WARNING("Failed to create face: hedge already have faces!");
                    return FacePtr();
                }
            }

            for (size_t i=0; i < count; ++i) {
                if (hedges[i]->vertex != hedges[(i+1)%count].pair()->vertex) {
                    LOG_WARNING("Failed to create face: hedges that dont' form a loop!");
                    return FacePtr();
                }
            }

            return createFace(impl, hedges, count);
        }

        FacePtr createFace( Impl *impl, const VertexHandle *handles, size_t vertexCount )
        {
            if (vertexCount < 3) {
                LOG_WARNING("Failed to create face: to few vertexes (%zu), a face must have atleast 3 vertexes", vertexCount, MAX_FACE_VERTEXES);
                return FacePtr();
            }

            if (vertexCount >= MAX_FACE_VERTEXES) {
                LOG_WARNING("Failed to create face: to many vertexes (%zu), increase MAX_FACE_VERTEXES (%i)", vertexCount, MAX_FACE_VERTEXES);
                return FacePtr();
            }

            VertexPtr vertexes[MAX_FACE_VERTEXES];
            // Check that all vertexes exists
            for (size_t i=0; i < vertexCount; ++i) {
                vertexes[i] = VertexPtr(impl, handles[i]);
                if (!vertexes[i]) {
                    LOG_WARNING("Failed to create face: invalid vertexes!");
                    return FacePtr();
                }

                if (!isFree(impl, vertexes[i])) {
                    LOG_WARNING("Failed to create face: vertexes must be free (have hedges that don't have faces)!");
                    return FacePtr();
                }
            }

            HEdgePtr hedges[MAX_FACE_VERTEXES];

            // check if its posible to create a face with the given vertexes
            for (size_t i=0; i < vertexCount; ++i) {
                VertexPtr &v1 = vertexes[i],
                          &v2 = vertexes[(i+1)%vertexCount];

                HEdgePtr hedge = findHEdge(impl, v1, v2);

                if (hedge && hedge->face) {
                    LOG_WARNING("Trying to create a face with hedges that already have a face!");
                    return FacePtr();
                }
                else {
                    hedges[i] = hedge;
                }
            }

            // Create edges
            for (size_t i=0; i < vertexCount; ++i) {
                if (!hedges[i]) {
                    EdgePtr edge = createEdge(impl, vertexes[i], vertexes[(i+1) % vertexCount]);

                    if (!edge) {
                        LOG_ERROR("Failed to create face: couldn't create a edge between the vertex (%u) and (%u)", (unsigned)handles[i], (unsigned)handles[(i+1)%vertexCount]);
                        return FacePtr();
                    }


                    hedges[i] = edge.hedge();
                    assert (hedges[i]->vertex == vertexes[(i+1) % vertexCount]);
                }
            }

            return createFace(impl, hedges, vertexCount);
        }
    }

    COMMON_API HalfEdgeMeshBase::HalfEdgeMeshBase()
    {
        internal::init(mImpl, this);
    }

    COMMON_API HalfEdgeMeshBase::~HalfEdgeMeshBase()
    {
        internal::destroy(mImpl);
    }
    
    
    COMMON_API HalfEdgeMeshBase::HalfEdgeMeshBase( const HalfEdgeMeshBase &copy )
    {
        *mImpl = *copy.mImpl;
        mImpl->this_ = this;
    }

    COMMON_API HalfEdgeMeshBase::HalfEdgeMeshBase( HalfEdgeMeshBase &&move )
    {
        *mImpl = std::move(*move.mImpl);
        mImpl->this_ = this;
    }
        
    COMMON_API HalfEdgeMeshBase& HalfEdgeMeshBase::operator = ( const HalfEdgeMeshBase &copy )
    {
        *mImpl = *copy.mImpl;
        mImpl->this_ = this;
        return *this;
    }

    COMMON_API HalfEdgeMeshBase& HalfEdgeMeshBase::operator = ( HalfEdgeMeshBase &&move )
    {
        *mImpl = std::move(*move.mImpl);
        mImpl->this_ = this;
        return *this;
    }

    COMMON_API void HalfEdgeMeshBase::clear()
    {
        internal::clear(mImpl);
    }

    COMMON_API HalfEdgeMeshBase::VertexHandle HalfEdgeMeshBase::createVertex()
    {
        return internal::createVertex(mImpl);
    }

    COMMON_API HalfEdgeMeshBase::EdgeHandle HalfEdgeMeshBase::createEdge( VertexHandle v1, VertexHandle v2 )
    {
        return internal::createEdge(mImpl, 
            internal::VertexPtr(mImpl, v1),
            internal::VertexPtr(mImpl, v2)
        );
    }
    
    COMMON_API HalfEdgeMeshBase::FaceHandle HalfEdgeMeshBase::createFace( const VertexHandle *vertexes, size_t vertexCount )
    {
        return internal::createFace(mImpl,
            vertexes, vertexCount                          
        );
    }

    COMMON_API HalfEdgeMeshBase::FaceHandle HalfEdgeMeshBase::createFace( const HEdgeHandle *hedges, size_t count )
    {
        return internal::createFace(mImpl,
            hedges, count                          
        );
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::findHEdge( VertexHandle v1, VertexHandle v2 ) const
    {
        return internal::findHEdge(mImpl, 
            internal::CVertexPtr(mImpl, v1),
            internal::CVertexPtr(mImpl, v2)
        );
    }

    COMMON_API HalfEdgeMeshBase::EdgeHandle HalfEdgeMeshBase::findEdge( VertexHandle v1, VertexHandle v2 ) const
    {
        return internal::findEdge(mImpl, 
            internal::CVertexPtr(mImpl, v1),
            internal::CVertexPtr(mImpl, v2)
        );
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::getHEdgePair( HEdgeHandle handle ) const
    {
        internal::CHEdgePtr hedge(mImpl, handle);
        if (hedge.valid()) return hedge->pair;
        return HEdgeHandle();
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::getHEdgeNext( HEdgeHandle handle ) const
    {
        internal::CHEdgePtr hedge(mImpl, handle);
        if (hedge.valid()) return hedge->next;
        return HEdgeHandle();
    }

    COMMON_API HalfEdgeMeshBase::VertexHandle HalfEdgeMeshBase::getHEdgeVertex( HEdgeHandle handle ) const
    {
        internal::CHEdgePtr hedge(mImpl, handle);
        if (hedge.valid()) return hedge->vertex;
        return VertexHandle();
    }

    COMMON_API HalfEdgeMeshBase::EdgeHandle HalfEdgeMeshBase::getHEdgeEdge( HEdgeHandle handle ) const
    {
        internal::CHEdgePtr hedge(mImpl, handle);
        if (hedge.valid()) return hedge->edge;
        return EdgeHandle();
    }

    COMMON_API HalfEdgeMeshBase::FaceHandle HalfEdgeMeshBase::getHEdgeFace( HEdgeHandle handle ) const
    {
        internal::CHEdgePtr hedge(mImpl, handle);
        if (hedge.valid()) return hedge->face;
        return FaceHandle();
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::getVertexHEdge( VertexHandle handle ) const
    {
        internal::CVertexPtr vertex(mImpl, handle);
        if (vertex.valid()) return vertex->hedge;
        return HEdgeHandle();
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::getEdgeHEdge( EdgeHandle handle ) const
    {
        internal::CEdgePtr edge(mImpl, handle);
        if (edge.valid()) return edge->hedge;
        return HEdgeHandle();
    }

    COMMON_API HalfEdgeMeshBase::HEdgeHandle HalfEdgeMeshBase::getFaceHEdge( FaceHandle handle ) const
    {
        internal::CFacePtr face(mImpl, handle);
        if (face.valid()) return face->hedge;
        return HEdgeHandle();
    }

    COMMON_API std::pair<HalfEdgeMeshBase::VertexHandle, HalfEdgeMeshBase::VertexHandle> HalfEdgeMeshBase::getEdgeVertexes( EdgeHandle handle ) const
    {
        internal::CEdgePtr edge(mImpl, handle);
        if (!edge.valid()) return {};

        auto h1 = edge.hedge();
        auto h2 = h1.pair();

        return {h1->vertex, h2->vertex};
    }

    COMMON_API std::pair<HalfEdgeMeshBase::FaceHandle, HalfEdgeMeshBase::FaceHandle> HalfEdgeMeshBase::getEdgeFaces( EdgeHandle handle ) const
    {
        internal::CEdgePtr edge(mImpl, handle);
        if (!edge.valid()) return {};

        auto h1 = edge.hedge();
        auto h2 = h1.pair();

        return {h1->face, h2->face};
    }

    COMMON_API bool HalfEdgeMeshBase::isBorderFace( FaceHandle handle ) const
    {
        internal::CFacePtr face(mImpl, handle);
        if (!face.valid()) return false;

        internal::CHEdgePtr hedge = face.hedge();

        do {
            internal::CHEdgePtr pair = hedge.pair();
            if (!pair->face) return true;
            
            hedge = hedge.next();
        } while (hedge != face->hedge);

        return false;
    }

#define _IMPLEMENT_CORE_ITER( Type, name )                                                  \
    COMMON_API HalfEdgeMeshBase::Type##Range HalfEdgeMeshBase::name() const {               \
        return Type##Range{name##Begin(), name##End()};                                     \
    }                                                                                       \
    COMMON_API HalfEdgeMeshBase::Type##Iterator HalfEdgeMeshBase::name##Begin() const {     \
        return Type##Iterator{*this};                                                       \
    }                                                                                       \
    COMMON_API HalfEdgeMeshBase::Type##Iterator HalfEdgeMeshBase::name##End() const {       \
        return Type##Iterator{*this, EndIterator()};                                        \
    }

    _IMPLEMENT_CORE_ITER(Vertex, vertexes);
    _IMPLEMENT_CORE_ITER(HEdge, hedges);
    _IMPLEMENT_CORE_ITER(Edge, edges);
    _IMPLEMENT_CORE_ITER(Face, faces);

#undef _IMPLEMENT_CORE_ITER

#define _IMPLEMENT_HEDGE_ITER(Type, name, Handle)                                                       \
    COMMON_API HalfEdgeMeshBase::Type##Range HalfEdgeMeshBase::name( Handle handle ) const {            \
        return Type##Range{name##Begin(handle), name##End(handle)};                                     \
    }                                                                                                   \
    COMMON_API HalfEdgeMeshBase::Type##Iterator HalfEdgeMeshBase::name##Begin( Handle handle ) const {  \
        return Type##Iterator{*this, handle};                                                           \
    }                                                                                                   \
    COMMON_API HalfEdgeMeshBase::Type##Iterator HalfEdgeMeshBase::name##End( Handle handle ) const {    \
        return Type##Iterator{EndIterator()};                                                           \
    }

    _IMPLEMENT_HEDGE_ITER(VertexVertex, vertexVertexes, VertexHandle);
    _IMPLEMENT_HEDGE_ITER(VertexOHEdge, vertexOHEdges, VertexHandle);
    _IMPLEMENT_HEDGE_ITER(VertexIHEdge, vertexIHEdges, VertexHandle);
    _IMPLEMENT_HEDGE_ITER(VertexEdge, vertexEdges, VertexHandle);
    _IMPLEMENT_HEDGE_ITER(VertexFace, vertexFaces, VertexHandle);

    _IMPLEMENT_HEDGE_ITER(FaceVertex, faceVertexes, FaceHandle);
    _IMPLEMENT_HEDGE_ITER(FaceHEdge, faceHEdges, FaceHandle);
    _IMPLEMENT_HEDGE_ITER(FaceEdge, faceEdges, FaceHandle);
    _IMPLEMENT_HEDGE_ITER(FaceFace, faceFaces, FaceHandle);

#undef _IMPLEMENT_HEDGE_ITER


}

#if COMMON_DEBUG_LEVEL > 0

#include "Common/ErrorUtils.h"

#include <sstream>

namespace Common
{
    namespace internal
    {
        void verifyInvariants( const Impl *impl )
        {
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr edge(impl, handle);
                    FATAL_ASSERT (edge.valid(), "");

                    FATAL_ASSERT (edge->pair);
                    FATAL_ASSERT (edge->next);

                    CHEdgePtr pair = edge.pair();
                    FATAL_ASSERT (pair.valid());
                    FATAL_ASSERT(pair->pair == edge);

                    FATAL_ASSERT(edge->edge);
                    FATAL_ASSERT(edge->edge == pair->edge);

                    CHEdgePtr next = edge.next();
                    FATAL_ASSERT(next.valid());
                    FATAL_ASSERT(next->prev == edge);
                }
            );

            impl->vertexes.forEachHandle(
                [&]( VertexHandle handle ) {
                    CVertexPtr vertex(impl, handle);
                    FATAL_ASSERT (vertex.valid());

                    if (vertex->hedge)
                    {
                        // Walk around the vertex
                        CHEdgePtr edge = vertex.hedge();
                        do {
                            FATAL_ASSERT(edge.valid());

                            CHEdgePtr pair = edge.pair();
                            FATAL_ASSERT(pair.valid());
                            FATAL_ASSERT(pair->vertex == handle);
                    
                            edge = pair.next();
                        } while (edge != vertex->hedge);
                    }
                }
            );
            impl->faces.forEachHandle(
                [&]( FaceHandle handle ) {
                    CFacePtr face(impl, handle);
                    FATAL_ASSERT(face.valid());

                    if (face->hedge) {
                        // Walk around the face
                        CHEdgePtr edge = face.hedge();

                        int count = 0;
                        do {
                            FATAL_ASSERT (edge.valid());
                            FATAL_ASSERT (edge->face == face);

                            edge = edge.next();
                            ++count;
                        } while (edge != face->hedge);

                        // A face must have atleast 3 vertexes
                        FATAL_ASSERT (count >= 3);
                    }
                }
            );

            // Check that all hedges can be reached from its vertex
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr hedge(impl, handle);
                    
                    CVertexPtr vertex = hedge.pair().vertex();
                    
                    hedge = vertex.hedge();
                    do {
                        // We found our edge
                        if (hedge == handle) return;

                        hedge = hedge.pairNext();

                    } while (hedge != vertex->hedge);
                    FATAL_ERROR("Half Edge %i isn't reacheble from vertex %i - but it says its origate from it!", (int)handle, (int)vertex.handle);
                }
            );

            // Check that all hedges can be reached from its face
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr hedge(impl, handle);

                    // Edge is on a border and doesn't have a face
                    if (!hedge->face) return;
                    
                    CFacePtr face = hedge.face();
                    
                    hedge = face.hedge();
                    do {
                        // We found our edge
                        if (hedge == handle) return;

                        hedge = hedge.next();

                    } while (hedge != face->hedge);
                    FATAL_ERROR("Half Edge %i isn't reacheble from face %i - but it says its a edge for it!", (int)handle, (int)face.handle);
                }
            );
        }

        void dumpInternalState( const Impl *impl )
        {
            printf("Internal state of HalfEdgeMesh %p\n", impl);
            printf("==================================================\n");

            printf("Half Edges:\n");
            printf("\t %6s | %6s | %6s | %6s | %6s | %6s\n", "ID", "pair", "next", "prev", "face", "vertex");

            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr edge(impl, handle);
                    FATAL_ASSERT (edge.valid());
                
                    printf ("\t %6i | %6i | %6i | %6i | %6i | %6i\n", (int)handle, (int)edge->pair, (int)edge->next, (int)edge->prev, (int)edge->face, (int)edge->vertex);
                }
            );

            printf("Vertexes:\n");
            printf("\t %6s | %6s\n", "id", "hedge");
            impl->vertexes.forEachHandle(
                [&]( VertexHandle handle ) {
                    CVertexPtr vertex(impl, handle);
                    FATAL_ASSERT (vertex.valid());

                    printf("\t %6i | %6i\n", (int)handle, (int)vertex->hedge);
                }
            );
        
            printf("Edges:\n");
            printf("\t %6s | %6s\n", "id", "hedge");
            impl->edges.forEachHandle(
                [&]( EdgeHandle handle ) {
                    CEdgePtr edge(impl, handle);
                    FATAL_ASSERT (edge.valid());
                
                    printf("\t %6i | %6i\n", (int)handle, (int)edge->hedge);
                }
            );
        
            printf("Faces:\n");
            printf("\t %6s | %6s\n", "id", "hedge");
            impl->faces.forEachHandle(
                [&]( FaceHandle handle ) {
                    CFacePtr face(impl, handle);
                    FATAL_ASSERT (face.valid());
                
                    printf("\t %6i | %6i\n", (int)handle, (int)face->hedge);

                }
            );
            printf("==================================================\n");
        }

        
        void dumpDotGraph( const Impl *impl )
        {
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    printf("H%i\n", (int)handle);
                }
            );
            impl->vertexes.forEachHandle(
                [&]( VertexHandle handle ) {
                    printf("V%i\n", (int)handle);
                }
            );
            
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    printf("H%i\n", (int)handle);
                }
            );
            
            impl->edges.forEachHandle(
                [&]( EdgeHandle handle ) {
                    CEdgePtr edge(impl, handle);
                    
                    CHEdgePtr hedge(edge.hedge());
                    CHEdgePtr pair(hedge.pair());

                    printf("H%i H%i\n", (int)hedge.handle, (int)pair.handle);
                    printf("H%i H%i\n", (int)pair.handle, (int)hedge.handle);
                }
            );
            
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr hedge(impl, handle);
                    CHEdgePtr pair(hedge.pair());

                    printf("V%i H%i\n", (int)pair->vertex, (int)hedge.handle);
                    printf("H%i V%i\n", (int)hedge.handle, (int)hedge->vertex);
                }
            );

            return;

            // @todo figure out a better way to do this 
            //   (the generated diagrams are a bit unclear)
            printf("digraph HalfEdgeMesh_%p {\n", impl);
            
            printf("  ");
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    printf("H%i;", (int)handle);
                }
            );
            printf("\n");
            printf("  ");
            impl->vertexes.forEachHandle(
                [&]( VertexHandle handle ) {
                    printf("V%i;", (int)handle);
                }
            );
            printf("\n");

            impl->edges.forEachHandle(
                [&]( EdgeHandle handle ) {
                    CEdgePtr edge(impl, handle);

                    CHEdgePtr hedge = edge.hedge();
                    CHEdgePtr pair = hedge.pair();

                    printf("  subgraph cluster_edge_%i {\n", (int)handle);
                    printf("    color=blue;\n");
                    printf("    edge [color=red];\n");
                    printf("    H%i -> H%i;\n", (int)hedge.handle, (int)pair.handle);
                    printf("    H%i -> H%i;\n", (int)pair.handle, (int)hedge.handle);
                    printf("  }\n");

                }
            );
            
            printf("  subgraph hedge_next {\n");
            printf("    edge [color=cyan];\n");
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr edge(impl, handle);

                    printf("    H%i -> H%i;\n", (int)edge.handle, (int)edge->next);
                }
            );
            printf("  }\n");
            
            printf("  subgraph hedge_vertex {\n");
            printf("    edge [color=green];\n");
            impl->hedges.forEachHandle(
                [&]( HEdgeHandle handle ) {
                    CHEdgePtr edge(impl, handle);

                    printf("    H%i -> V%i;\n", (int)edge.handle, (int)edge->vertex);
                }
            );
            printf("  }\n");

            printf("}\n");
        }

    }

    COMMON_API void HalfEdgeMeshBase::verifyInvariants() const
    {
        internal::verifyInvariants(mImpl);
    }

    COMMON_API void HalfEdgeMeshBase::dumpInternalState() const
    {
        internal::dumpInternalState(mImpl);
    }

    COMMON_API void HalfEdgeMeshBase::dumpDotGraph() const
    {
        internal::dumpDotGraph(mImpl);
    }
}
#endif