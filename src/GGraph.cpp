#include "GGraph.hpp"

// Define NULL_GEDGE
const GEdge GEdge::NULL_GEDGE = GEdge(Edge::NULL_EDGE, GradePoint(-1, -1));

// Constructor
GGraph::GGraph(int n) { gVertices.reserve(n); }

void GGraph::add_vertex(Vertex v, GradePoint grade) {
    gVertices.emplace_back(GVertex{v, grade});
    adjacency.emplace_back(std::vector<EdgeId>());
}

GGraph::GGraph(int nVertices, const std::vector<GradePoint> &node_grades,
               const std::vector<std::pair<int, int>> &edges_input,
               const std::vector<GradePoint> &edge_grades) {
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
}

void GGraph::add_vertex(VertexId v, GradePoint grade) {
    add_vertex(Vertex(v, v), grade);
}
void GGraph::add_vertex(VertexId v) { add_vertex(Vertex(v, v)); }
void GGraph::add_vertex(Vertex v) { add_vertex(v, GradePoint(-1, -1)); }

// Method to add an edge to the graph
void GGraph::add_edge(VertexId v, VertexId w, GradePoint grade_idx) {
    if (v > w) {
        std::swap(v, w);
    }
    int id = gEdges.size();
    gEdges.emplace_back(GEdge{Edge(v, w, id), grade_idx});
    adjacency[v].push_back(id);
    adjacency[w].push_back(id);
}

void GGraph::add_edge(VertexId v, VertexId w) {
    add_edge(v, w, GradePoint(-1, -1));
}

void GGraph::add_edge(Vertex v, Vertex w, GradePoint grade_idx) {
    add_edge(v.id, w.id, grade_idx);
}

void GGraph::add_edge(Vertex v, Vertex w) { add_edge(v.id, w.id); }

const VAdj &GGraph::get_adj(VertexId v) const {
    if (is_vertex_removed(v)) {
        throw std::runtime_error(
            "vertex " + std::to_string(v) +
            " is removed, but trying to get its adjacency list");
    }
    return adjacency[v];
}

VAdj &GGraph::get_adj(VertexId v) {
    if (is_vertex_removed(v)) {
        throw std::runtime_error(
            "vertex " + std::to_string(v) +
            " is removed, but trying to get its adjacency list");
    }
    return adjacency[v];
}

// Remove edge e
void GGraph::remove_edge(EdgeId id) {
    const auto &e = gEdges[id].e;
    VertexId v0 = e.get_v0();
    VertexId v1 = e.get_v1();
    // Check if the edge exists in edges
    if (id != -1) {  // remove it
        relabel_edge_for_removal(id);
        remove_from_adjacency(v0, id);
        remove_from_adjacency(v1, id);
    } else {
        std::cout << "Try to remove edge, but " << std::endl;
        throw std::runtime_error("Edge " + std::to_string(e[0]) + ", " +
                                 std::to_string(e[1]) + " does not exist.");
    }
}

// Update graph by a vertex dictionary v->v_new:
// if v_new = v, then nothing is done
// otherwise, v is removed and v_new is kept and adjacency of v is merged into
// v_new Note: merge in adjacency is unsafe due to duplicate edges, and it is
// left to user's responsibility
void GGraph::update_graph(std::vector<size_t> vert_map) {
    // check if vert_dict has the correct size
    assert(vert_map.size() == gVertices.size() &&
           "vert_map.size() != gVertices.size()");

    // update edges
    for (auto &ge : gEdges) {
        Edge &e = ge.e;
        e[0] = gVertices[vert_map[e[0]]].v.id;
        e[1] = gVertices[vert_map[e[1]]].v.id;
    }

    // update adjacency and relabel vertices when vert_dict[i] != i
    for (size_t i = 0; i < vert_map.size(); ++i) {
        if (vert_map[i] != i) {
            // merge adjacency of v to v_new
            merge_adjacency_sets(gVertices[i].v.id,
                                 gVertices[vert_map[i]].v.id);
            // relabel v_i
            relabel_vertex_for_removal(i);
            active_vertices_flags[i] = false;
        }
    }
}

size_t GGraph::get_size_of_active_grades() const {
    // collect all active grades
    std::vector<GradePoint> grades_collection;
    for (const auto &gv : gVertices) {
        if (gv.get_id() == -1) {
            continue;
        }
        grades_collection.push_back(gv.get_grade());
    }
    for (const auto &ge : gEdges) {
        if (ge.get_id() == -1) {
            continue;
        }
        grades_collection.push_back(ge.get_grade());
    }
    // sort and remove duplicates
    std::sort(grades_collection.begin(), grades_collection.end(),
              LexicographicalOrderGradePoint());

    // remove duplicates
    grades_collection.erase(
        std::unique(grades_collection.begin(), grades_collection.end()),
        grades_collection.end());
    return grades_collection.size();
}

void GGraph::print_sizes() const {
    std::cout << "Graph size: " << gVertices.size() << std::endl;
    std::cout << "Edge size: " << gEdges.size() << std::endl;
    std::cout << "Adjacency size: " << adjacency.size() << std::endl;
}

void GGraph::print_size_of_adjacency() const {
    size_t size_vertices = 0;
    size_t size_edges = 0;
    for (size_t i = 0; i < adjacency.size(); ++i) {
        if (gVertices[i].get_id() == -1) {
            continue;
        }  // skip removed vertices
        size_vertices += 1;
        size_edges += adjacency[i].size();
    }
    std::cout << "\tSize of adjacency list (nV): " << size_vertices
              << std::endl;
    std::cout << "\tSize of edges (nE * 2): " << size_edges << std::endl;
}

void GGraph::print_graph_info() const {
    std::cout << "Graph info: " << std::endl;
    std::cout << "\tvertices: " << std::endl;
    for (const auto &v : gVertices) {
        std::cout << "\t\t" << v.get_id() << " " << v.get_grade() << std::endl;
    }
    std::cout << "\tedges: id (v0, v1) grade" << std::endl;
    for (const auto &e : gEdges) {
        std::cout << "\t\t" << e.get_id() << " (" << e.get_v0() << ", "
                  << e.get_v1() << ") " << e.get_grade() << std::endl;
    }
    std::cout << "\tadjacency list: adj[v] = {e1, e2, ...}" << std::endl;
    for (size_t i = 0; i < adjacency.size(); ++i) {
        std::cout << "\t\t adj[" << i << "] = ";
        for (const auto &e : adjacency[i]) {
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
}

void GGraph::merge_adjacency_sets(VertexId v, VertexId u) {
    // Check if both keys exist in the map
    if (is_vertex_removed(v)) {
        throw std::runtime_error("vertex " + std::to_string(v) +
                                 " is removed, but trying to merge into it");
    }
    if (is_vertex_removed(u)) {
        throw std::runtime_error("vertex " + std::to_string(u) +
                                 " is removed, but trying to merge into it");
    }
    if (u == v) {
        throw std::runtime_error("Trying to merge vertex " + std::to_string(u) +
                                 " into itself");
    }

    // Merge adjacency of u to v
    adjacency[u].insert(adjacency[u].end(), adjacency[v].begin(),
                        adjacency[v].end());

    // remove duplicates from adjacency[u] (unnecessary)
    // std::sort(adjacency[u].begin(), adjacency[u].end());
    // adjacency[u].erase(std::unique(adjacency[u].begin(), adjacency[u].end()),
    // adjacency[u].end());

    // remove v from adjacency
    std::vector<EdgeId>().swap(adjacency[v]);
}

void GGraph::remove_from_adjacency(VertexId v, EdgeId id) {
    // Check if the key exists in the map
    if (is_vertex_removed(v)) {
        throw std::runtime_error(
            "vertex " + std::to_string(v) +
            " is removed, but trying to remove an edge from it");
    }

    // Find the edge id in the adjacency list of v
    auto it = std::find(adjacency[v].begin(), adjacency[v].end(), id);
    if (it == adjacency[v].end()) {
        throw std::runtime_error("edge " + std::to_string(id) +
                                 " not found in the adjacency list of vertex " +
                                 std::to_string(v));
    }

    // Erase the element
    // adjacency[v].erase(it);
    std::swap(adjacency[v].back(), *it);
    adjacency[v].pop_back();
}

std::vector<VertexId> GGraph::get_active_vertices_ids() const {
    std::vector<VertexId> active_vertices_ids;
    active_vertices_ids.reserve(gVertices.size());
    for (size_t i = 0; i < active_vertices_flags.size(); ++i) {
        if (active_vertices_flags[i]) {
            active_vertices_ids.push_back(i);
        }
    }
    return active_vertices_ids;
}
