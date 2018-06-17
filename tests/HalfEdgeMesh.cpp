#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Common/HalfEdgeMesh.h"
#include "Common/HalfEdgeMeshIterator.h"

#include <vector>

struct Vertex {
    int data = 0;
};

struct HalfEdge {
    int data = 0;
};
struct Edge {
    int data = 0;
};
struct Face {
    int data = 0;
};


using HalfEdgeMesh = Common::HalfEdgeMesh<Vertex, HalfEdge, Edge, Face>;
using VertexHandle = HalfEdgeMesh::VertexHandle;
using HEdgeHandle = HalfEdgeMesh::HEdgeHandle;
using EdgeHandle = HalfEdgeMesh::EdgeHandle;
using FaceHandle = HalfEdgeMesh::FaceHandle;

struct EdgeInfo {
    EdgeHandle handle;
    VertexHandle v1, v2;
};
struct FaceInfo {
    FaceHandle handle;
    std::vector<VertexHandle> vertexes;
};




void verifyMesh( const HalfEdgeMesh &mesh, 
    std::vector<VertexHandle> vertexes,
    std::vector<EdgeInfo> edges,
    std::vector<FaceInfo> faces
)
{
    for (EdgeInfo &edge : edges) {
        auto handle = mesh.findEdge(edge.v1, edge.v2);
        if (!edge.handle) {
            REQUIRE(handle);
            edge.handle = handle;
        }
        else {
            REQUIRE(handle == edge.handle);
        }
    }

    for (const FaceInfo &face : faces) {
        HalfEdgeMesh::HEdgeHandle edge = mesh.findHEdge(
            face.vertexes[0],   
            face.vertexes[1]
        );
        REQUIRE(edge);

        for (int i=1; i <= face.vertexes.size(); ++i) {
            REQUIRE(mesh.getHEdgeFace(edge) == face.handle);
            HalfEdgeMesh::VertexHandle vertex = face.vertexes[i % face.vertexes.size()];

            HalfEdgeMesh::VertexHandle edgeVert = mesh.getHEdgeVertex(edge);
            REQUIRE(edgeVert == vertex);

            edge = mesh.getHEdgeNext(edge);
            REQUIRE(edge);
        }
    }
    SECTION("Iterators")
    {
        { // Vertexes
            std::set<VertexHandle> visited;

            for (VertexHandle handle : mesh.vertexes()) {
                REQUIRE(visited.count(handle) == 0);
                visited.insert(handle);
            }

            REQUIRE(visited.size() == vertexes.size());
            for (VertexHandle handle : vertexes) {
                REQUIRE(visited.count(handle) == 1);
            }
        }
        { // HEdges
            std::set<HEdgeHandle> visited;

            for (HEdgeHandle handle : mesh.hedges()) {
                REQUIRE(visited.count(handle) == 0);
                visited.insert(handle);
            }

            REQUIRE(visited.size() == edges.size()*2);
            for (const auto &edge : edges) {
                HEdgeHandle hedge = mesh.getEdgeHEdge(edge.handle);
                HEdgeHandle pair = mesh.getHEdgePair(hedge);

                REQUIRE(visited.count(hedge) == 1);
                REQUIRE(visited.count(pair) == 1);
            }
        }
        { // Edges
            std::set<EdgeHandle> visited;

            for (EdgeHandle handle : mesh.edges()) {
                REQUIRE(visited.count(handle) == 0);
                visited.insert(handle);
            }

            REQUIRE(visited.size() == edges.size());
            for (const auto &edge : edges) {
                REQUIRE(visited.count(edge.handle) == 1);
            }
        }
        { // Faces
            std::set<FaceHandle> visited;

            for (FaceHandle handle : mesh.faces()) {
                REQUIRE(visited.count(handle) == 0);
                visited.insert(handle);
            }

            REQUIRE(visited.size() == faces.size());
            for (const auto &face : faces) {
                REQUIRE(visited.count(face.handle) == 1);
            }
        }
        { // VertexVertex
            for (VertexHandle vertex : vertexes) {
                std::vector<VertexHandle> handles;

                for (const auto &edge : edges) {
                    if (edge.v1 == vertex) {
                        handles.push_back(edge.v2);
                    }
                    if (edge.v2 == vertex) {
                        handles.push_back(edge.v1);
                    }
                }

                std::set<VertexHandle> visited;
                for (VertexHandle handle : mesh.vertexVertexes(vertex)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }
                
                REQUIRE(visited.size() == handles.size());
                for (const auto &vertex : handles) {
                    REQUIRE(visited.count(vertex) == 1);
                }
            }
        }
        { // VertexOHEdge
            for (VertexHandle vertex : vertexes) {
                std::vector<HEdgeHandle> handles;

                for (const auto &edge : edges) {
                    if (edge.v1 != vertex && edge.v2 != vertex) continue;

                    HEdgeHandle hedge = mesh.getEdgeHEdge(edge.handle);
                    if (mesh.getHEdgeVertex(hedge) != vertex) {
                        handles.push_back(hedge);
                    }
                    else {
                        handles.push_back(mesh.getHEdgePair(hedge));
                    }
                }

                std::set<HEdgeHandle> visited;
                for (auto handle : mesh.vertexOHEdges(vertex)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }
                
                REQUIRE(visited.size() == handles.size());
                for (const auto &handle : handles) {
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // VertexIHEdge
            for (VertexHandle vertex : vertexes) {
                std::vector<HEdgeHandle> handles;

                for (const auto &edge : edges) {
                    if (edge.v1 != vertex && edge.v2 != vertex) continue;

                    HEdgeHandle hedge = mesh.getEdgeHEdge(edge.handle);
                    if (mesh.getHEdgeVertex(hedge) == vertex) {
                        handles.push_back(hedge);
                    }
                    else {
                        handles.push_back(mesh.getHEdgePair(hedge));
                    }
                }

                std::set<HEdgeHandle> visited;
                for (auto handle : mesh.vertexIHEdges(vertex)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }
                
                REQUIRE(visited.size() == handles.size());
                for (const auto &handle : handles) {
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // VertexEdge
            for (VertexHandle vertex : vertexes) {
                std::vector<EdgeHandle> handles;

                for (const auto &edge : edges) {
                    if (edge.v1 == vertex || edge.v2 == vertex) {
                        handles.push_back(edge.handle);
                    }
                }

                std::set<EdgeHandle> visited;
                for (auto handle : mesh.vertexEdges(vertex)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }
                
                REQUIRE(visited.size() == handles.size());
                for (const auto &handle : handles) {
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // FaceVertex
            for (const FaceInfo &info : faces) {
                std::set<VertexHandle> visited;
                for (auto handle : mesh.faceVertexes(info.handle)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }

                REQUIRE(visited.size() == info.vertexes.size());
                for (const auto &handle : info.vertexes) {
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // FaceHEdge
            for (const FaceInfo &info : faces) {
                std::set<HEdgeHandle> visited;
                for (auto handle : mesh.faceHEdges(info.handle)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }

                REQUIRE(visited.size() == info.vertexes.size());
                for (size_t i=0, s=info.vertexes.size(); i < s; ++i) {
                    HEdgeHandle handle = mesh.findHEdge(info.vertexes[i], info.vertexes[(i+1)%s]);
                    REQUIRE (handle);
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // FaceEdge
            for (const FaceInfo &info : faces) {
                std::set<EdgeHandle> visited;
                for (auto handle : mesh.faceEdges(info.handle)) {
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }

                REQUIRE(visited.size() == info.vertexes.size());
                for (size_t i=0, s=info.vertexes.size(); i < s; ++i) {
                    EdgeHandle handle = mesh.findEdge(info.vertexes[i], info.vertexes[(i+1)%s]);
                    REQUIRE (handle);
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
        { // FaceFace 
            for (const FaceInfo &info : faces) {
                std::set<FaceHandle> neighbors;
                
                for (size_t i=0, s=info.vertexes.size(); i < s; ++i) {
                    VertexHandle v1 = info.vertexes[i],
                                 v2 = info.vertexes[(i+1)%s];

                    for (const FaceInfo &face : faces) {
                        if (face.handle == info.handle) continue;

                        for (size_t a=0,b=face.vertexes.size(); a < b; ++a) {
                            if (face.vertexes[a] == v2 && face.vertexes[(a+1)%b] == v1) {
                                REQUIRE(neighbors.count(face.handle) == 0);
                                neighbors.insert(face.handle);
                            }
                        }
                    }
                }


                std::set<FaceHandle> visited;
                for (FaceHandle handle : mesh.faceFaces(info.handle)) {
                    REQUIRE(handle);
                    REQUIRE(visited.count(handle) == 0);
                    visited.insert(handle);
                }
                
                REQUIRE(visited.size() == neighbors.size());
                for (auto handle : neighbors) {
                    REQUIRE(visited.count(handle) == 1);
                }
            }
        }
    }
}

TEST_CASE( "HalfEdgeMesh", "[Common][HalfEdgeMesh]" )
{
    HalfEdgeMesh mesh;
     

    SECTION("Edges") 
    {
        /*
            1 -- 2
            |  /
            | /
            3    4
        */

        auto v1 = mesh.createVertex(),
             v2 = mesh.createVertex(),
             v3 = mesh.createVertex(),
             v4 = mesh.createVertex();

        auto e1 = mesh.createEdge(v1, v2),
             e2 = mesh.createEdge(v2, v3),
             e3 = mesh.createEdge(v3, v1);

        REQUIRE(e1);
        REQUIRE(e2);
        REQUIRE(e3);

#if COMMON_DEBUG_LEVEL > 0
        mesh.verifyInvariants();
#endif
        
        verifyMesh(mesh,
            {v1,v2,v3,v4},
            {{e1,v1,v2}, {e2,v2,v3}, {e3,v3,v1}},
            {}
        );
    }

    SECTION("Faces")
    {
        /*
                  3      4
                  | \  /   \ <=f2
              f1=>|  1------5
                  |/  \     
                  2    6
        */

        auto v1 = mesh.createVertex(),
             v2 = mesh.createVertex(),
             v3 = mesh.createVertex(),
             v4 = mesh.createVertex(),
             v5 = mesh.createVertex(),
             v6 = mesh.createVertex();

        REQUIRE(v1);
        REQUIRE(v2);
        REQUIRE(v3);
        REQUIRE(v4);
        REQUIRE(v5);
        REQUIRE(v6);

        auto e1 = mesh.createEdge(v1, v2),
             e2 = mesh.createEdge(v1, v6);

        REQUIRE(e1);
        REQUIRE(e2);
#if COMMON_DEBUG_LEVEL > 0
        mesh.verifyInvariants();
#endif
        auto f1Vertexes = {v1, v2, v3};
        auto f1 = mesh.createFace(f1Vertexes.begin(), f1Vertexes.size());
        REQUIRE(f1);
#if COMMON_DEBUG_LEVEL > 0
        mesh.verifyInvariants();
#endif

        auto f2Vertexes = {v1, v4, v5};
        auto f2 = mesh.createFace(f2Vertexes.begin(), f2Vertexes.size());
        REQUIRE(f2);
            
#if COMMON_DEBUG_LEVEL > 0
        mesh.verifyInvariants();
#endif
        /*
                  3   6  4
                  | \ | /   \ <=f2
              f1=>|  1------5
                  |/    ___/ <=f3
                  2----/
        */

        auto f3Vertexes = {v2,v1,v5};
        auto f3 = mesh.createFace(f3Vertexes.begin(), f3Vertexes.size());
        
        REQUIRE(f3);
        

        auto f4Vertexes = {v1,v3,v4};
        auto f4 = mesh.createFace(f4Vertexes.begin(), f4Vertexes.size());
        REQUIRE(!f4);

        verifyMesh(mesh,
            {v1, v2, v3, v4, v5, v6},
            {{e1,v1, v2}, {{}, v2, v3}, {{}, v3,v1}, {e2, v1, v6}, {{}, v1, v4}, {{}, v4, v5}, {{}, v1, v5}, {{}, v5, v2}, {{}, v3, v4}},
            {{f1, {v1, v2, v3}}, {f2, {v1, v4, v5}}, {f3, {v2, v1, v5}}}
        );

#if COMMON_DEBUG_LEVEL > 0
        mesh.verifyInvariants();
#endif
    }
}
