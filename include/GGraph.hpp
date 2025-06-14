#pragma once
#include "Edge.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <algorithm> // Include this for std::reverse
#include <unordered_map> // Include this for edge values
#include <boost/functional/hash.hpp> // Include this for Boost hash
#include <utility> // Include this for std::pair
#include <icecream.hpp>
#include <numeric> // Include this for std::iota
#include "Grade.hpp"

/*
    (Graded) Graph class
    ------------------------------------------------------------
    Every edge or vertex has its filtration value on GradeTable

    Note that vertices are initialized as [0, 1, 2, ..., n-1], but it will change after collapse
*/
// graded vertex
struct GVertex {
    Vertex v; GradePoint grade;

    GVertex(Vertex v, GradePoint grade): v(v), grade(grade) {}
    GVertex(): v(Vertex()), grade(GradePoint(-1, -1)) {}

    // getter methods
    inline VertexId get_id() const {return v.id;}
    inline VertexId get_label() const {return v.label;}
    inline const GradePoint& get_grade() const {return grade;}

    // equality operator
    bool operator==(const GVertex& other) const {return v == other.v && grade == other.grade;}
};

// graded edge
struct GEdge {
    Edge e;
    GradePoint grade;
    static const GEdge NULL_GEDGE; // null edge
    GEdge(Edge e, GradePoint grade): e(e), grade(grade) {}
    GEdge(): e(Edge::NULL_EDGE), grade(GradePoint(-1, -1)) {}

    // getter methods
    inline EdgeId get_id() const {return e.get_id();}
    inline VertexId get_v0() const {return e.get_v0();}
    inline VertexId get_v1() const {return e.get_v1();}
    inline const GradePoint& get_grade() const {return grade;}

    // equality operator
    bool operator==(const GEdge& other) const {return e == other.e && grade == other.grade;}
    bool operator!=(const GEdge& other) const {return !(*this == other);}
};
const GEdge GEdge::NULL_GEDGE = GEdge(Edge::NULL_EDGE, GradePoint(-1, -1));

class GGraph {
public:
    GGraph() = default;

    // Constructor reserve sizes 
    GGraph(int n);

    // Copy Constructor
    GGraph(const GGraph&) = default;

    // Move Constructor
    GGraph(GGraph&&) = default;

    // Copy Assignment
    GGraph& operator=(const GGraph&) = default;

    // Move Assignment
    GGraph& operator=(GGraph&&) = default;

    // vector constructor
    GGraph(
        int nVertices, 
        const std::vector<GradePoint>& node_grades, 
        const std::vector<std::pair<int, int>>& edges_input, 
        const std::vector<GradePoint>& edge_grades) {

        // Initialize vertices by filling with 0, 1, 2, ..., n-1
        gVertices.reserve(nVertices);
        adjacency.reserve(nVertices);
        active_vertices_flags.resize(nVertices, true);
        for (int i = 0; i < nVertices; ++i) {
            gVertices.emplace_back(GVertex{Vertex(i, i), node_grades[i]});
            adjacency.emplace_back(std::vector<EdgeId>());
        }

        // Initialize edges and create adjacency list
        for (size_t i = 0; i < edges_input.size(); ++i) {
            add_edge(edges_input[i].first, edges_input[i].second, edge_grades[i]);
        }
        security_check_adjacency_map();
        security_check_shapes();
    }

    // get # of vertices
    inline int get_nvertices() const {return gVertices.size();};

    // get # of edges
    inline int get_nedges() const {return gEdges.size();};

    // print sizes of all data structures
    void print_sizes() const {
        std::cout << "Graph size: " << gVertices.size() << std::endl;
        std::cout << "Edge size: " << gEdges.size() << std::endl;
        std::cout << "Adjacency size: " << adjacency.size() << std::endl;
    }
    
    // getter functions
    inline const std::vector<GVertex>& get_gvertices() const {return gVertices;};
    inline const std::vector<GEdge>& get_gedges() const {return gEdges;};

    //  get adjacency list at vertex v
    const VAdj& get_adj(VertexId v) const;
    VAdj& get_adj(VertexId v);

    //  get edge and vertex by its id
    inline const Edge& get_edge(EdgeId id) const {return gEdges[id].e;};
    inline const GEdge& get_gedge(EdgeId id) const {return gEdges[id];};
    inline const GVertex& get_gvertex(size_t v_idx) const {return gVertices[v_idx];};
    inline const Vertex& get_vertex(size_t v_idx) const {return gVertices[v_idx].v;};

    // get active vertices ids
    std::vector<VertexId> get_active_vertices_ids() const {
        std::vector<VertexId> active_vertices_ids;
        active_vertices_ids.reserve(gVertices.size());
        for (size_t i = 0; i < active_vertices_flags.size(); ++i) {
            if (active_vertices_flags[i]) {
                active_vertices_ids.push_back(i);
            }
        }
        return active_vertices_ids;
    };

    // find the size of active grades
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
    inline void remove_edge(Edge e){remove_edge(e.get_id());}

    // Update graph based on a vertex dictionary v->v_new, keeping the image v_new
    void update_graph(std::vector<size_t> vert_dict);

    // print the size of adjacency list
    void print_size_of_adjacency() const {
        size_t size_vertices = 0;
        size_t size_edges = 0;
        for (size_t i = 0; i < adjacency.size(); ++i) {
            if (gVertices[i].get_id() == -1) {continue;} // skip removed vertices
            size_vertices += 1;
            size_edges += adjacency[i].size();
        }
        std::cout << "\tSize of adjacency list (nV): " << size_vertices << std::endl;
        std::cout << "\tSize of edges (nE * 2): " << size_edges << std::endl;
    }

    void print_graph_info() const {
        std::cout << "Graph info: " << std::endl;
        std::cout << "\tvertices: " << std::endl;
        for (const auto& v: gVertices) {
            std::cout << "\t\t" << v.get_id() << " " << v.get_grade() << std::endl;
        }
        std::cout << "\tedges: id (v0, v1) grade" << std::endl;
        for (const auto& e: gEdges) {
            std::cout << "\t\t" << e.get_id() << " (" << e.get_v0() << ", " << e.get_v1() << ") " << e.get_grade() << std::endl;
        }
        std::cout << "\tadjacency list: adj[v] = {e1, e2, ...}" << std::endl;
        for (size_t i = 0; i < adjacency.size(); ++i) {
            std::cout << "\t\t adj[" << i << "] = ";
            for (const auto& e: adjacency[i]) {
                std::cout << e << " ";
            }
            std::cout << std::endl;
        }
    }

    // initialize active vertices flags with true
    void initialize_active_vertices_flags() {
        active_vertices_flags.resize(gVertices.size(), true);
    }

private:
    EdgeId edge_id_assign = 0; // used to assign next edge an unique identity
    std::vector<GVertex> gVertices;  // collections of graded vertices 
    std::vector<GEdge> gEdges; // collections of graded edges 
    // adjacency list to represent the graph, map vertex to std::vector<EdgeId>
    std::vector<VAdj> adjacency;
    std::vector<bool> active_vertices_flags;

    // relabel vertex v to -1 for removal
    inline void relabel_vertex_for_removal(size_t v_idx){
        GVertex& gv = gVertices[v_idx];
        gv.v.id = -1; gv.grade.x = -1; gv.grade.y = -1;
    }

    // relabel edge e to -1 for removal
    inline void relabel_edge_for_removal(size_t e_idx){
        GEdge& ge = gEdges[e_idx];
        ge.e.set_id(-1); ge.grade.x = -1; ge.grade.y = -1;
    }

    inline bool is_vertex_removed(size_t v_idx) const {
        return gVertices[v_idx].get_id() == -1;
    }

    // Merge u into v: all neighbors of u become neighbors of v, and v is removed
    void merge_adjacency_sets(VertexId v, VertexId u) {

        // Check if both keys exist in the map
        if (is_vertex_removed(v)) {
            throw std::runtime_error("vertex " + std::to_string(v) + " is removed, but trying to merge into it");
        }
        if (is_vertex_removed(u)){ 
            throw std::runtime_error("vertex " + std::to_string(u) + " is removed, but trying to merge into it");
        }
        if (u == v) {
            throw std::runtime_error("Trying to merge vertex " + std::to_string(u) + " into itself");
        }

        // Merge adjacency of u to v
        adjacency[u].insert(adjacency[u].end(), adjacency[v].begin(), adjacency[v].end());

        // remove duplicates from adjacency[u]
        // std::sort(adjacency[u].begin(), adjacency[u].end());
        // adjacency[u].erase(std::unique(adjacency[u].begin(), adjacency[u].end()), adjacency[u].end());

        // remove v from adjacency
        std::vector<EdgeId>().swap(adjacency[v]);
    }

    // Helper function to remove an edge from the adjacency hash map
    void remove_from_adjacency(VertexId v, EdgeId id) {
        // Check if the key exists in the map
        if (is_vertex_removed(v)) {
            throw std::runtime_error("vertex " + std::to_string(v) + " is removed, but trying to remove an edge from it");
        }

        // Find the edge id in the adjacency list of v
        auto it = std::find(adjacency[v].begin(), adjacency[v].end(), id);
        if (it == adjacency[v].end()) {
            throw std::runtime_error("edge " + std::to_string(id) + " not found in the adjacency list of vertex " + std::to_string(v));
        }

        // Erase the element
        //adjacency[v].erase(it);
        std::swap(adjacency[v].back(), *it);
        adjacency[v].pop_back();
    }

    #ifdef ENABLE_SECURITY_CHECKS
    void security_check_adjacency_map(){
        for (const auto& vertex : gVertices) {
            // Check if the vertex is in the adjacency_map
            if (adjacency.find(vertex) == adjacency.end()) {
                // If not, add the vertex with an empty vector as the value
                adjacency[vertex] = std::vector<EdgeId>();
            }
        }
    }
    #else
    void security_check_adjacency_map(){
        // do nothing
    }
    #endif

    #ifdef ENABLE_SECURITY_CHECKS
    void security_check_shapes(){
        assert(gVertices.size() == vert_values.size() && "vertices.size() !=  vert_values.size()");
        assert(gVertices.size() == adjacency.size() && "vertices.size() !=  adjacency.size()");
        assert(gEdges.size() == edge_values.size() && "edges.size() !=  edge_values.size()");
    }
    #else
    void security_check_shapes(){
        // do nothing
    }
    #endif
};


// Constructor
GGraph::GGraph(int n) {gVertices.reserve(n);}

void GGraph::add_vertex(Vertex v, GradePoint grade){
    gVertices.emplace_back(GVertex{v, grade});
    adjacency.emplace_back(std::vector<EdgeId>());
}

void GGraph::add_vertex(VertexId v, GradePoint grade){add_vertex(Vertex(v, v), grade);}
void GGraph::add_vertex(VertexId v){add_vertex(Vertex(v, v));}
void GGraph::add_vertex(Vertex v){add_vertex(v, GradePoint(-1, -1));}


// Method to add an edge to the graph
void GGraph::add_edge(VertexId v, VertexId w, GradePoint grade_idx) {
    if (v > w) {
        std::swap(v, w);
    }
    int id = gEdges.size();
    gEdges.emplace_back(GEdge{Edge(v, w, id), grade_idx});
    adjacency[v].push_back(id);
    adjacency[w].push_back(id);
    security_check_adjacency_map();
}

void GGraph::add_edge(VertexId v, VertexId w) {
    add_edge(v, w, GradePoint(-1, -1));
}

void GGraph::add_edge(Vertex v, Vertex w, GradePoint grade_idx) {
    add_edge(v.id, w.id, grade_idx);
}

void GGraph::add_edge(Vertex v, Vertex w) {
    add_edge(v.id, w.id);
}

const VAdj& GGraph::get_adj(VertexId v) const {
    if (is_vertex_removed(v)) {
        throw std::runtime_error("vertex " + std::to_string(v) + " is removed, but trying to get its adjacency list");
    }
    return adjacency[v];
}

VAdj& GGraph::get_adj(VertexId v) {
    if (is_vertex_removed(v)) {
        throw std::runtime_error("vertex " + std::to_string(v) + " is removed, but trying to get its adjacency list");
    }
    return adjacency[v];
}

// Remove edge e 
void GGraph::remove_edge(EdgeId id){
    const auto& e = gEdges[id].e;
    VertexId v0 = e.get_v0();
    VertexId v1 = e.get_v1();
    // Check if the edge exists in edges
    if (id != -1) { // remove it
        relabel_edge_for_removal(id);
        remove_from_adjacency(v0, id);
        remove_from_adjacency(v1, id);
    } else {
        std::cout << "Try to remove edge, but "<< std::endl;
        throw std::runtime_error("Edge " + std::to_string(e[0]) + ", " + std::to_string(e[1]) + " does not exist.");
    }
}


// Update graph by a vertex dictionary v->v_new:
// if v_new = v, then nothing is done
// otherwise, v is removed and v_new is kept and adjacency of v is merged into v_new
// Note: merge in adjacency is unsafe due to duplicate edges, and it is left to user's responsibility
void GGraph::update_graph(std::vector<size_t> vert_map){
    // check if vert_dict has the correct size
    assert(vert_map.size() == gVertices.size() && "vert_map.size() != gVertices.size()");
    
    // update edges 
    for (auto& ge: gEdges){
        Edge& e = ge.e;
        e[0] = gVertices[vert_map[e[0]]].v.id;
        e[1] = gVertices[vert_map[e[1]]].v.id;
    }

    // update adjacency and relabel vertices when vert_dict[i] != i
    for (size_t i = 0; i < vert_map.size(); ++i){
        if (vert_map[i] != i) {
            // merge adjacency of v to v_new
            merge_adjacency_sets(gVertices[i].v.id, gVertices[vert_map[i]].v.id);
            // relabel v_i
            relabel_vertex_for_removal(i);
            active_vertices_flags[i] = false;
        }
    }
}

size_t GGraph::get_size_of_active_grades() const {
    // collect all active grades
    std::vector<GradePoint> grades_collection;
    for (const auto& gv: gVertices){
        if (gv.get_id() == -1) {continue;}
        grades_collection.push_back(gv.get_grade());
    }
    for (const auto& ge: gEdges){
        if (ge.get_id() == -1) {continue;}
        grades_collection.push_back(ge.get_grade());
    }
    // sort and remove duplicates
    std::sort(grades_collection.begin(), grades_collection.end(), LexicographicalOrderGradePoint());

    // remove duplicates
    grades_collection.erase(std::unique(grades_collection.begin(), grades_collection.end()), grades_collection.end());
    return grades_collection.size();
}