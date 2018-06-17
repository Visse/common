#include "HalfEdgeMeshIterator.h"
#include "HalfEdgeMesh.impl.h"



namespace Common
{
    namespace HalfEdgeMeshIterators
    {
        using namespace Common::internal;

        using MeshImpl = internal::HalfEdgeMeshBaseImpl;

#define IMPLEMENT_CORE_ITER(Type, Vector, Handle, member)                           \
        struct Type::Impl {                                                         \
            Vector::const_iterator iter;                                            \
        };                                                                          \
        COMMON_API Type::Type( const HalfEdgeMeshBase &mesh ) {                     \
            const MeshImpl *info = getImpl(mesh);                                   \
            mImpl->iter = info->member.begin();                                     \
        }                                                                           \
        COMMON_API Type::Type( const HalfEdgeMeshBase &mesh, EndIterator end ) {    \
            const MeshImpl *info = getImpl(mesh);                                   \
            mImpl->iter = info->member.end();                                       \
        }                                                                           \
        COMMON_API Type::Type( const Type &copy ) {                                 \
            mImpl->iter = copy.mImpl->iter;                                         \
        }                                                                           \
        COMMON_API Type::~Type() {}                                                 \
        COMMON_API Type& Type::operator = ( const Type &copy ) {                    \
            mImpl->iter = copy.mImpl->iter;                                         \
            return *this;                                                           \
        }                                                                           \
        COMMON_API bool Type::equal( const Type &other ) const {                    \
            return mImpl->iter == other.mImpl->iter;                                \
        }                                                                           \
        COMMON_API Handle Type::dereference() const {                               \
            return mImpl->iter.handle();                                            \
        }                                                                           \
        COMMON_API void Type::increment() {                                         \
            ++mImpl->iter;                                                          \
        }                                                                           \
        COMMON_API void Type::decrement() {                                         \
            --mImpl->iter;                                                          \
        }

        IMPLEMENT_CORE_ITER(VertexIterator, VertexVector, VertexHandle, vertexes);
        IMPLEMENT_CORE_ITER(HEdgeIterator, HEdgeVector, HEdgeHandle, hedges);
        IMPLEMENT_CORE_ITER(EdgeIterator, EdgeVector, EdgeHandle, edges);
        IMPLEMENT_CORE_ITER(FaceIterator, FaceVector, FaceHandle, faces);
#undef IMPLEMENT_CORE_ITER

        
#define IMPLEMENT_HEDGE_ITER(Type, SmartPtr, InitHandle, Handle, inc, get)          \
        struct Type::Impl {                                                         \
            CHEdgePtr hedge;                                                        \
            HEdgeHandle head;                                                       \
            int lap = 0;                                                            \
        };                                                                          \
        COMMON_API Type::Type( const HalfEdgeMeshBase &mesh, InitHandle handle ) {  \
            const HalfEdgeMeshBaseImpl *impl = getImpl(mesh);                       \
            SmartPtr head(impl, handle);                                            \
            if (!head) {                                                            \
                mImpl->lap = 1;                                                     \
                return;                                                             \
            }                                                                       \
            mImpl->hedge = head.hedge();                                            \
            if (!mImpl->hedge) {                                                    \
                mImpl->lap = 1;                                                     \
                return;                                                             \
            }                                                                       \
            while (!(mImpl->hedge get)) {                                           \
                mImpl->hedge = mImpl->hedge inc;                                    \
                if (mImpl->hedge == head->hedge) {                                  \
                    mImpl->lap = 1;                                                 \
                    mImpl->hedge = CHEdgePtr();                                     \
                    break;                                                          \
                }                                                                   \
            }                                                                       \
            mImpl->head = mImpl->hedge;                                             \
        }                                                                           \
        COMMON_API Type::Type( EndIterator end ) {                                  \
            mImpl->lap = 1;                                                         \
        }                                                                           \
        COMMON_API Type::Type( const Type &copy ) {                                 \
            *this = copy;                                                           \
        }                                                                           \
        COMMON_API Type::~Type() {}                                                 \
        COMMON_API Type& Type::operator = ( const Type &copy ) {                    \
            mImpl->hedge = copy.mImpl->hedge;                                       \
            mImpl->head = copy.mImpl->head;                                         \
            mImpl->lap = copy.mImpl->lap;                                           \
            return *this;                                                           \
        }                                                                           \
        COMMON_API bool Type::equal( const Type &other ) const {                    \
            if (mImpl->lap != other.mImpl->lap) return false;                       \
            if (mImpl->hedge.valid() && other.mImpl->hedge.valid()) {               \
                return mImpl->hedge == other.mImpl->hedge;                          \
            }                                                                       \
            return true;                                                            \
        }                                                                           \
        COMMON_API Handle Type::dereference() const {                               \
            if (!mImpl->hedge) return Handle();                                     \
            return mImpl->hedge get;                                                \
        }                                                                           \
        COMMON_API void Type::increment() {                                         \
            if (!mImpl->hedge) return;                                              \
            do {                                                                    \
                mImpl->hedge = mImpl->hedge inc;                                    \
                if (mImpl->hedge == mImpl->head) {                                  \
                    mImpl->lap++;                                                   \
                    return;                                                         \
                }                                                                   \
            } while (!(mImpl->hedge get));                                          \
        }
        
        IMPLEMENT_HEDGE_ITER(VertexVertexIterator, CVertexPtr, VertexHandle, VertexHandle, .pairNext(), ->vertex);
        IMPLEMENT_HEDGE_ITER(VertexOHEdgeIterator, CVertexPtr, VertexHandle, HEdgeHandle, .pairNext(), .handle);
        IMPLEMENT_HEDGE_ITER(VertexIHEdgeIterator, CVertexPtr, VertexHandle, HEdgeHandle, .pairNext(), ->pair);
        IMPLEMENT_HEDGE_ITER(VertexEdgeIterator, CVertexPtr, VertexHandle, EdgeHandle, .pairNext(), ->edge);
        
        IMPLEMENT_HEDGE_ITER(FaceVertexIterator, CFacePtr, FaceHandle, VertexHandle, .next(), ->vertex);
        IMPLEMENT_HEDGE_ITER(FaceHEdgeIterator, CFacePtr, FaceHandle, HEdgeHandle, .next(), .handle);
        IMPLEMENT_HEDGE_ITER(FaceEdgeIterator, CFacePtr, FaceHandle, EdgeHandle, .next(), ->edge);

        IMPLEMENT_HEDGE_ITER(FaceFaceIterator, CFacePtr, FaceHandle, FaceHandle, .next(), .pair()->face);

#undef IMPLEMENT_HEDGE_ITER
    }
}