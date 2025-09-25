#pragma once
#include <algorithm>                  // Include this for std::reverse
#include <boost/functional/hash.hpp>  // Include this for Boost hash
#include <icecream.hpp>
#include <iostream>
#include <list>
#include <numeric>  // Include this for std::iota
#include <stack>
#include <unordered_map>  // Include this for edge values
#include <utility>        // Include this for std::pair
#include <vector>

#include "Edge.hpp"
#include "Grade.hpp"
#include "Vertex.hpp"

/*
    (Graded) Graph class
    ------------------------------------------------------------
    Every edge or vertex has its filtration value on GradeTable

    Note that vertices are initialized as [0, 1, 2, ..., n-1], but it will change after
   collapse
*/
// graded vertex
struct GVertex {
    Vertex v;
    GradePoint grade;

    GVertex(Vertex v, GradePoint grade) : v(v), grade(grade) {}
    GVertex() : v(Vertex()), grade(GradePoint(-1, -1)) {}

    // getter methods
    inline VertexId get_id() const { return v.id; }
    inline VertexId get_label() const { return v.label; }
    inline const GradePoint &get_grade() const { return grade; }

    // equality operator
    bool operator==(const GVertex &other) const { return v == other.v && grade == other.grade; }
};

// graded edge
struct GEdge {
    Edge e;
    GradePoint grade;
    static const GEdge NULL_GEDGE;  // null edge
    GEdge(Edge e, GradePoint grade) : e(e), grade(grade) {}
    GEdge() : e(Edge::NULL_EDGE), grade(GradePoint(-1, -1)) {}

    // getter methods
    inline EdgeId get_id() const { return e.get_id(); }
    inline VertexId get_v0() const { return e.get_v0(); }
    inline VertexId get_v1() const { return e.get_v1(); }
    inline const GradePoint &get_grade() const { return grade; }

    // equality operator
    bool operator==(const GEdge &other) const { return e == other.e && grade == other.grade; }
    bool operator!=(const GEdge &other) const { return !(*this == other); }
};

class GGraph {
   public:
    GGraph() = default;

    // Constructor reserve sizes
    GGraph(int n);

    // Copy Constructor
    GGraph(const GGraph &) = default;

    // Move Constructor
    GGraph(GGraph &&) = default;

    // Copy Assignment
    GGraph &operator=(const GGraph &) = default;

    // Move Assignment
    GGraph &operator=(GGraph &&) = default;

    // vector constructor
    GGraph(int nVertices,                                        // number of vertices
           const std::vector<GradePoint> &node_grades,           // grades of vertices
           const std::vector<std::pair<int, int>> &edges_input,  // edges: (v0, v1)
           const std::vector<GradePoint> &edge_grades            // grades of edges
    );

    // get # of vertices
    inline int get_nvertices() const { return gVertices.size(); };

    // get # of edges
    inline int get_nedges() const { return gEdges.size(); };

    // getter functions
    inline const std::vector<GVertex> &get_gvertices() const { return gVertices; };
    inline const std::vector<GEdge> &get_gedges() const { return gEdges; };

    //  get adjacency list at vertex v
    const VAdj &get_adj(VertexId v) const;
    VAdj &get_adj(VertexId v);

    //  get edge and vertex by its id
    inline const Edge &get_edge(EdgeId id) const { return gEdges[id].e; };
    inline const GEdge &get_gedge(EdgeId id) const { return gEdges[id]; };
    inline const GVertex &get_gvertex(size_t v_idx) const { return gVertices[v_idx]; };
    inline const Vertex &get_vertex(size_t v_idx) const { return gVertices[v_idx].v; };

    // get active vertices ids
    std::vector<VertexId> get_active_vertices_ids() const;

    // find the size of active grades (expensive operation)
    size_t get_size_of_active_grades() const;

    // add some vertices and edges to the graph
    void add_vertex(VertexId v, GradePoint grade);
    void add_vertex(VertexId v);
    void add_vertex(Vertex v, GradePoint grade);
    void add_vertex(Vertex v);
    void add_edge(VertexId v_id, VertexId w_id, GradePoint grade);
    void add_edge(VertexId v_id, VertexId w_id);
    void add_edge(Vertex v, Vertex w, GradePoint grade);
    void add_edge(Vertex v, Vertex w);

    // Remove edge e
    void remove_edge(EdgeId id);
    inline void remove_edge(Edge e) { remove_edge(e.get_id()); }

    // Update graph based on a vertex dictionary v->v_new, keeping the image v_new
    void update_graph(std::vector<size_t> vert_dict);

    // print functions
    void print_sizes() const;
    void print_size_of_adjacency() const;
    void print_graph_info() const;

    // initialize active vertices flags with true
    inline void initialize_active_vertices_flags() {
        active_vertices_flags.resize(gVertices.size(), true);
    }

   private:
    EdgeId edge_id_assign = 0;       // used to assign next edge an unique identity
    std::vector<GVertex> gVertices;  // collections of graded vertices
    std::vector<GEdge> gEdges;       // collections of graded edges
    // Caution: after collapse, the adjacency list may contain duplicates,
    // which is fine if using `visited` to perform DFS
    std::vector<VAdj> adjacency;  // adjacency list: vertex to std::vector<EdgeId>
    std::vector<bool>
        active_vertices_flags;  // flags to indicate if the vertex is active or removed

    // relabel vertex v and edge e to -1 for removal
    inline void relabel_vertex_for_removal(size_t v_idx) {
        GVertex &gv = gVertices[v_idx];
        gv.v.id = -1;
        gv.grade.x = -1;
        gv.grade.y = -1;
    }
    inline void relabel_edge_for_removal(size_t e_idx) {
        GEdge &ge = gEdges[e_idx];
        ge.e.set_id(-1);
        ge.grade.x = -1;
        ge.grade.y = -1;
    }

    inline bool is_vertex_removed(size_t v_idx) const { return gVertices[v_idx].get_id() == -1; }

    // Merge u into v: all neighbors of u become neighbors of v, and v is removed
    void merge_adjacency_sets(VertexId v, VertexId u);

    // Helper function to remove an edge from the adjacency list
    void remove_from_adjacency(VertexId v, EdgeId id);
};
