#include "HalfEdgeMesh.h"
#include "HashTable.h"

#include <cassert>

namespace Common
{
    namespace internal 
    {
        using VertexHandle = HalfEdgeMeshBase::VertexHandle;
        using HEdgeHandle = HalfEdgeMeshBase::HEdgeHandle;
        using EdgeHandle = HalfEdgeMeshBase::EdgeHandle;
        using FaceHandle = HalfEdgeMeshBase::FaceHandle;

        struct HEdge {
            HEdgeHandle pair,
                        next,
                        prev;
            VertexHandle vertex;
            FaceHandle face;
            EdgeHandle edge;
        };
        struct Vertex {
            HEdgeHandle hedge;
        };
        struct Edge {
            HEdgeHandle hedge;
        };
        struct Face {
            HEdgeHandle hedge;
        };

        using VertexVector = HandleVector<VertexHandle, Vertex>;
        using HEdgeVector = HandleVector<HEdgeHandle, HEdge>;
        using EdgeVector = HandleVector<EdgeHandle, Edge>;
        using FaceVector = HandleVector<FaceHandle, Face>;


        struct HalfEdgeMeshBaseImpl {
            HalfEdgeMeshBase *this_;

            // These *Lock vars is for making sure our smart handle's have cached a valid pointer
            //  (pointers can be invalidated if new items are allocated, or old ones free'd)
            // In those cases the lock variable is incremented and the next time the smart handle 
            // tries to use its cached pointer it will refrech it
            uint8_t vertexesLock = 0,
                    hedgesLock = 0,
                    edgesLock = 0,
                    facesLock = 0;

            VertexVector vertexes;
            HEdgeVector hedges;
            EdgeVector edges;
            FaceVector faces;

            
            void onVertexCreated( VertexHandle handle )  {
                this_->onVertexCreated(handle);
            }
            void onHEdgeCreated( HEdgeHandle handle )  {
                this_->onHEdgeCreated(handle);
            }
            void onEdgeCreated( EdgeHandle handle )  {
                this_->onEdgeCreated(handle);
            }
            void onFaceCreated( FaceHandle handle )  {
                this_->onFaceCreated(handle);
            }
         
            void onVertexDestroyed( VertexHandle handle )  {
                this_->onVertexDestroyed(handle);
            }
            void onHEdgeDestroyed( HEdgeHandle handle )  {
                this_->onHEdgeDestroyed(handle);
            }
            void onEdgeDestroyed( EdgeHandle handle )  {
                this_->onEdgeDestroyed(handle);
            }
            void onFaceDestroyed( FaceHandle handle )  {
                this_->onFaceDestroyed(handle);
            }
        };
        
        using Impl = HalfEdgeMeshBaseImpl;
        

        namespace SmartHandle
        {
            // Smart handles enables a easier to chase pointers around
            // They have protection agaist pointer invalidation,
            // by tracking if any new objects are allocated or free'ing

#define CREATE_SMART_HANDLE_CONTENT(Name, Member, Type, Handle, const_)         \
            const_ Impl *impl = nullptr;                                        \
            mutable Handle handle;                                              \
            mutable const_ Type *ptr = nullptr;                                 \
            mutable uint8_t lock = 0;                                           \
                                                                                \
            Name() = default;                                                   \
            Name( const Name& ) = default;                                      \
            Name& operator = ( const Name& ) = default;                         \
            Name( const_ Impl *impl_, Handle handle_ ) :                        \
                impl(impl_),                                                    \
                handle(handle_)                                                 \
            {                                                                   \
            }                                                                   \
            Name( const_ Impl *impl_, Handle handle_,                           \
                  const_ Type *ptr_, uint8_t lock_ ) :                          \
                impl(impl_),                                                    \
                handle(handle_),                                                \
                ptr(ptr_),                                                      \
                lock(lock_)                                                     \
            {                                                                   \
            }                                                                   \
            explicit operator bool () const {                                   \
                return valid();                                                 \
            }                                                                   \
            operator Handle () const {                                          \
                return handle;                                                  \
            }                                                                   \
            const_ Type* get() const {                                          \
                if (impl == nullptr) return nullptr;                            \
                if (!handle) return nullptr;                                    \
                if (impl->Member##Lock != lock) ptr = nullptr;                  \
                if (ptr) return ptr;                                            \
                ptr = impl->Member.find(handle);                                \
                if (!ptr) handle = Handle();                                    \
                lock = impl->Member##Lock;                                      \
                return ptr;                                                     \
            }                                                                   \
            const_ Type* operator -> () const {                                 \
                const_ Type *ptr = get();                                       \
                assert (ptr);                                                   \
                return ptr;                                                     \
            }                                                                   \
            bool valid() const {                                                \
                return get() != nullptr;                                        \
            }                                                                   \
            friend bool operator == ( const Name &lhs, const Name &rhs ) {      \
                return lhs.handle == rhs.handle;                                \
            }                                                                   \
            friend bool operator == ( const Handle &lhs, const Name &rhs ) {    \
                return lhs == rhs.handle;                                       \
            }                                                                   \
            friend bool operator == ( const Name &lhs, const Handle &rhs ) {    \
                return lhs.handle == rhs;                                       \
            }                                                                   \
        

            struct CVertexPtr;
            struct CEdgePtr;
            struct CFacePtr;
        
            struct CHEdgePtr {
                CREATE_SMART_HANDLE_CONTENT(CHEdgePtr, hedges, HEdge, HEdgeHandle, const);

                CHEdgePtr next() const {
                    if (valid()) return CHEdgePtr{impl, get()->next};
                    return CHEdgePtr();
                }
                CHEdgePtr prev() const {
                    if (valid()) return CHEdgePtr{impl, get()->next};
                    return CHEdgePtr();
                }
                CHEdgePtr pair() const {
                    if (valid()) return CHEdgePtr{impl, get()->pair};
                    return CHEdgePtr();
                }
            
                CHEdgePtr pairNext() const {
                    if (valid()) {
                        return pair().next();
                    }
                    return CHEdgePtr();
                }

                CHEdgePtr nextPair() const {
                    if (valid()) {
                        return next().pair();
                    }
                    return CHEdgePtr();
                }
            
                CVertexPtr vertex() const;
                CEdgePtr edge() const;
                CFacePtr face() const;
            };

            struct CVertexPtr {
                CREATE_SMART_HANDLE_CONTENT(CVertexPtr, vertexes, Vertex, VertexHandle, const);

                CHEdgePtr hedge() const {
                    if (valid()) return CHEdgePtr{impl, get()->hedge};
                    return CHEdgePtr();
                }
            };
            
            struct CEdgePtr {
                CREATE_SMART_HANDLE_CONTENT(CEdgePtr, edges, Edge, EdgeHandle, const);

                CHEdgePtr hedge() const {
                    if (valid()) return CHEdgePtr{impl, get()->hedge};
                    return CHEdgePtr();
                }
            };

            struct CFacePtr {
                CREATE_SMART_HANDLE_CONTENT(CFacePtr, faces, Face, FaceHandle, const);
                
                CHEdgePtr hedge() const {
                    if (valid()) return CHEdgePtr{impl, get()->hedge};
                    return CHEdgePtr();
                }
            };

            inline CVertexPtr CHEdgePtr::vertex() const {
                if (valid()) return CVertexPtr{impl, get()->vertex};
                return CVertexPtr();
            }
            inline CEdgePtr CHEdgePtr::edge() const {
                if (valid()) return CEdgePtr{impl, get()->edge};
                return CEdgePtr();
            }
            inline CFacePtr CHEdgePtr::face() const {
                if (valid()) return CFacePtr{impl, get()->face};
                return CFacePtr();
            }


            struct VertexPtr;
            struct EdgePtr;
            struct FacePtr;
        
    #define nonconst
            struct HEdgePtr {
                CREATE_SMART_HANDLE_CONTENT(HEdgePtr, hedges, HEdge, HEdgeHandle, nonconst);
            
                operator CHEdgePtr () const {
                    return CHEdgePtr{impl, handle, ptr, lock};
                }

                HEdgePtr next() const {
                    if (valid()) return HEdgePtr{impl, get()->next};
                    return HEdgePtr();
                }
                HEdgePtr prev() const {
                    if (valid()) return HEdgePtr{impl, get()->prev};
                    return HEdgePtr();
                }
                HEdgePtr pair() const {
                    if (valid()) return HEdgePtr{impl, get()->pair};
                    return HEdgePtr();
                }
            
                HEdgePtr pairNext() const {
                    if (valid()) {
                        return pair().next();
                    }
                    return HEdgePtr();
                }
                
                HEdgePtr nextPair() const {
                    if (valid()) {
                        return next().pair();
                    }
                    return HEdgePtr();
                }
            
                VertexPtr vertex() const;
                EdgePtr edge() const;
                FacePtr face() const;
            };

            struct VertexPtr {
                CREATE_SMART_HANDLE_CONTENT(VertexPtr, vertexes, Vertex, VertexHandle, nonconst);
            
                operator CVertexPtr () const {
                    return CVertexPtr{impl, handle, ptr, lock};
                }

                HEdgePtr hedge() const {
                    if (valid()) return HEdgePtr{impl, get()->hedge};
                    return HEdgePtr();
                }
            };
        
            struct EdgePtr {
                CREATE_SMART_HANDLE_CONTENT(EdgePtr, edges, Edge, EdgeHandle, nonconst);

                operator CEdgePtr () const {
                    return CEdgePtr{impl, handle, ptr, lock};
                }

                HEdgePtr hedge() const {
                    if (valid()) return HEdgePtr{impl, get()->hedge};
                    return HEdgePtr();
                }
            };

            struct FacePtr {
                CREATE_SMART_HANDLE_CONTENT(FacePtr, faces, Face, FaceHandle, nonconst);
            
                operator CFacePtr () const {
                    return CFacePtr{impl, handle, ptr, lock};
                }

                HEdgePtr hedge() const {
                    if (valid()) return HEdgePtr{impl, get()->hedge};
                    return HEdgePtr();
                }
            };
    #undef nonconst
        
            inline VertexPtr HEdgePtr::vertex() const {
                if (valid()) return VertexPtr{impl, get()->vertex};
                return VertexPtr();
            }
            inline EdgePtr HEdgePtr::edge() const {
                if (valid()) return EdgePtr{impl, get()->edge};
                return EdgePtr();
            }
            inline FacePtr HEdgePtr::face() const {
                if (valid()) return FacePtr{impl, get()->face};
                return FacePtr();
            }

            inline VertexPtr asNonConst( const CVertexPtr &ptr )
            {
                return VertexPtr(
                    const_cast<Impl*>(ptr.impl),
                    ptr.handle,
                    const_cast<Vertex*>(ptr.ptr),
                    ptr.lock
                );
            }

            inline HEdgePtr asNonConst( const CHEdgePtr &ptr )
            {
                return HEdgePtr(
                    const_cast<Impl*>(ptr.impl),
                    ptr.handle,
                    const_cast<HEdge*>(ptr.ptr),
                    ptr.lock
                );
            }

            inline EdgePtr asNonConst( const CEdgePtr &ptr )
            {
                return EdgePtr(
                    const_cast<Impl*>(ptr.impl),
                    ptr.handle,
                    const_cast<Edge*>(ptr.ptr),
                    ptr.lock
                );
            }

            inline FacePtr asNonConst( const CFacePtr &ptr )
            {
                return FacePtr(
                    const_cast<Impl*>(ptr.impl),
                    ptr.handle,
                    const_cast<Face*>(ptr.ptr),
                    ptr.lock
                );
            }

    #undef CREATE_SMART_HANDLE_CONTENT
        }
        
        using SmartHandle::CVertexPtr;
        using SmartHandle::CHEdgePtr;
        using SmartHandle::CEdgePtr;
        using SmartHandle::CFacePtr;

        using SmartHandle::VertexPtr;
        using SmartHandle::HEdgePtr;
        using SmartHandle::EdgePtr;
        using SmartHandle::FacePtr;

        using SmartHandle::asNonConst;

        void init( Impl *impl, HalfEdgeMeshBase *this_ );
        void destroy( Impl *impl );

        VertexHandle createVertex( Impl *impl );

        CHEdgePtr findFreeHEdge( const Impl *impl, CHEdgePtr hedge );
        HEdgePtr findFreeHEdge( Impl *impl, HEdgePtr hedge );

        HEdgePtr findFreeHEdge( Impl *impl, HEdgePtr before, HEdgePtr after );

        bool isFree( Impl *impl, VertexPtr vertex );

        CHEdgePtr findHEdge( const Impl *impl, CVertexPtr v1, CVertexPtr v2 );
        HEdgePtr findHEdge( Impl *impl, VertexPtr v1, VertexPtr v2 );

        CEdgePtr findEdge( const Impl *impl, CVertexPtr v1, CVertexPtr v2 );
        EdgePtr findEdge( Impl *impl, VertexPtr v1, VertexPtr v2 );

        EdgePtr createEdge( Impl *impl, VertexPtr v1, VertexPtr v2 );

        bool makeAdjacent( Impl *impl, HEdgePtr in, HEdgePtr out );

        FacePtr createFace( Impl *impl, HEdgePtr *edges, size_t count );
        FacePtr createFace( Impl *impl, const HEdgeHandle *handles, size_t count );
        FacePtr createFace( Impl *impl, const VertexHandle *handles, size_t vertexCount );


        // Need dedicated allocate and free functions to
        // make sure that the lock is updated accordingly
#define CREATE_ALLOCATE_AND_FREE( Type, TypePtr, Handle, Member )       \
        inline TypePtr allocate##Type ( Impl *impl ) {                  \
            Handle handle = impl->Member.emplace();                     \
            if (handle) {                                               \
                impl->Member##Lock++;                                   \
            }                                                           \
            return TypePtr{impl, handle};                               \
        }                                                               \
        inline void free##Type( Impl *impl, Handle handle ) {           \
            if (!handle) return;                                        \
            impl->Member.free(handle);                                  \
            impl->Member##Lock++;                                       \
        }

        CREATE_ALLOCATE_AND_FREE(Vertex, VertexPtr, VertexHandle, vertexes);
        CREATE_ALLOCATE_AND_FREE(HEdge, HEdgePtr, HEdgeHandle, hedges);
        CREATE_ALLOCATE_AND_FREE(Edge, EdgePtr, EdgeHandle, edges);
        CREATE_ALLOCATE_AND_FREE(Face, FacePtr, FaceHandle, faces);

#undef CREATE_ALLOCATE_AND_FREE
    }
}