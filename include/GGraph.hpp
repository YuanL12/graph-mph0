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

class GGraph {
public:

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
        vertices.resize(nVertices);
        std::iota(vertices.begin(), vertices.end(), 0);

        // Initialize vertex grades
        vert_grades.reserve(nVertices);
        for (size_t i = 0; i < nVertices; ++i) {
            vert_grades[Vertex(i)] = node_grades[i];
        }

        // Initialize edges and create adjacency list
        for (size_t i = 0; i < edges_input.size(); ++i) {
            add_edge(edges_input[i].first, edges_input[i].second, edge_grades[i]);
        }
        security_check_adjacency_map();
        security_check_shapes();
    }

    // get # of vertices
    inline int get_nvertices() const {return vertices.size();};

    // get # of edges
    inline int get_nedges() const {return edges.size();};

    // print sizes of all data structures
    void print_sizes() const {
        std::cout << "Graph size: " << vertices.size() << std::endl;
        std::cout << "Edge size: " << edges.size() << std::endl;
        std::cout << "Adjacency size: " << adjacency.size() << std::endl;
        std::cout << "Edge values size: " << edge_grades.size() << std::endl;
        std::cout << "Vertex values size: " << vert_grades.size() << std::endl;
    }
    
    //  get all vertices
    inline const std::vector<int>& get_vertices() const {return vertices;};

    //  get adjacency list at vertex v
    VAdj get_adj(Vertex v) const;

    //  get edge by its id
    Edge get_edge(EdgeId id) const;

    // get all edge grades 
    inline const std::unordered_map<EdgeId, GradePoint>& get_edges_grades() const {return edge_grades;};

    // get all vertices grades 
    inline const std::unordered_map<Vertex, GradePoint>& get_vert_grades() const {return vert_grades;};

    // get edge and vertex grades 
    GradePoint get_edge_grade(EdgeId i) const {return edge_grades.at(i);};
    GradePoint get_edge_grade(Edge e) const {return edge_grades.at(e.get_id());};
    GradePoint get_vertex_grade(Vertex v) const {return vert_grades.at(v);};

    // add an edge to the graph
    void add_edge(Vertex v, Vertex w, GradePoint grade);
    void add_edge(Vertex v, Vertex w); // default grade is -1

    // add multiple edges to the graph
    void add_edges(const std::vector<std::tuple<int, int, GradePoint>>& fil_edges);

    // add a vertex
    void add_vertex(int v, GradePoint grade);

    // print the adjacency list
    void print_adjacency() const;
    
    // Remove edge e
    void remove_edge(Edge e);

    // Update graph based on a vertex dictionary v->v_new, keeping the codomain v_new
    void update_graph(std::unordered_map<Vertex, Vertex> vert_dict);

    // Method for Depth-First Search
    void DFS(Vertex startVertex) const;

    // Helper function to remove an edge from the adjacency hash map
    void remove_from_adjacency(Vertex v, EdgeId edgeId) {
        // Check if the key exists in the map
        auto it = adjacency.find(v);
        assert(it != adjacency.end() && "vertex not found in adjacency.");

        // Get reference to the vector
        std::vector<EdgeId>& vec = it->second;

        // Find the element in the vector
        auto vec_it = std::find(vec.begin(), vec.end(), edgeId);
        assert(vec_it != vec.end() && "edge not found in the vector.");

        // Erase the element
        vec.erase(vec_it);        
    }

private:
    EdgeId edge_id_assign = 0; // used to assign next edge an unique identity
    std::vector<Vertex> vertices; 
    std::unordered_map<Vertex, GradePoint> vert_grades; 
    std::unordered_map<EdgeId, Edge> edges;
    std::unordered_map<EdgeId, GradePoint> edge_grades; 
    // adjacency list to represent the graph, map vertex to edge Id  
    std::unordered_map<Vertex, VAdj> adjacency; 
    

    // merge vector of key1 to the vector of key2 and delete key1 later 
    template<typename keyT, typename VectT>
    void merge_keys_vectors(std::unordered_map<keyT, std::vector<VectT>>& map, keyT key1, keyT key2) {
        // Check if both keys exist in the map
        auto it1 = map.find(key1);
        auto it2 = map.find(key2);

        assert(it1 != map.end() && "Key1 not found in the map.");
        assert(it2 != map.end() && "Key2 not found in the map.");

        // Append the vector of key1 to the vector of key2
        it2->second.insert(it2->second.end(), it1->second.begin(), it1->second.end());

        // Optionally, you can erase key1 from the map if it's no longer needed
        map.erase(it1);
    }

    // Method to print the stack
    void printStack(const std::stack<int>& stack) const;

    #ifdef ENABLE_SECURITY_CHECKS
    void security_check_adjacency_map(){
        for (const auto& vertex : vertices) {
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
        assert(vertices.size() == vert_values.size() && "vertices.size() !=  vert_values.size()");
        assert(vertices.size() == adjacency.size() && "vertices.size() !=  adjacency.size()");
        assert(edges.size() == edge_values.size() && "edges.size() !=  edge_values.size()");
    }
    #else
    void security_check_shapes(){
        // do nothing
    }
    #endif
};


// Constructor
GGraph::GGraph(int n) {vertices.reserve(n);}

// // Copy Constructor
// GGraph::GGraph(const GGraph& other) 
//     : edge_id_assign(other.edge_id_assign),
//       vertices(other.vertices),
//       edges(other.edges), 
//       vert_grades(other.vert_grades),
//       edge_grades(other.edge_grades),
//       adjacency(other.adjacency)
// {security_check_shapes();}


// Method to add an edge to the graph
void GGraph::add_edge(Vertex v, Vertex w, GradePoint grade_idx) {
    if (v > w) {
        std::swap(v, w);
    }
    Edge edge = Edge(v, w, edge_id_assign);
    edges[edge_id_assign] = edge;
    edge_grades[edge_id_assign] = grade_idx;
    adjacency[v].emplace_back(edge_id_assign);
    adjacency[w].emplace_back(edge_id_assign);
    edge_id_assign++;
    security_check_adjacency_map();
}

void GGraph::add_edges(const std::vector<std::tuple<int, int, GradePoint>>& fil_edges) {
    this->edges.reserve(this->edges.size() + fil_edges.size());
    for (const auto& [v, w, grade_idx] : fil_edges)
        this->add_edge(v, w, grade_idx);
}

// Overloaded method to add an edge with default value 0.0
void GGraph::add_edge(Vertex v, Vertex w) {
    add_edge(v, w, GradePoint(-1, -1));
}

VAdj GGraph::get_adj(Vertex v) const {
    auto it = adjacency.find(v);
    if(it == adjacency.end()){
        throw std::runtime_error("vertex " + std::to_string(v) + " not found when trying to get its adjacency list");
    }
    return it->second;
}

Edge GGraph::get_edge(EdgeId id) const {
    auto it = edges.find(id);
    if(it == edges.end()){
        std::cout << "id = " << id << " not found when trying to get the edge" << std::endl;
        throw std::runtime_error("id not found when trying to get the edge");
    }
    return it->second; // return the edge
}
    

// Remove edge e 
void GGraph::remove_edge(Edge e){
    Vertex v0 = e.get_v0();
    Vertex v1 = e.get_v1();
    EdgeId id = e.get_id();
    // Check if the edge exists in edges
    if (edges.find(id) != edges.end()) { // remove it
        edges.erase(id);
        edge_grades.erase(id);
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
// It is unsafe b/c we don't check if edge use the vertex to be removed 
void GGraph::update_graph(std::unordered_map<Vertex, Vertex> vert_dict){
    // update vertices
    std::vector<Vertex> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto& p: vert_dict){
        if (p.first == p.second){
            new_vertices.emplace_back(p.first);
        }else{ // v is removed and v_new is kept
            // remove v from vertices grades
            vert_grades.erase(p.first);
        }
    }
    vertices = new_vertices;

    // update edges 
    for (auto& p: edges){
        Edge& e = p.second;
        e[0] = vert_dict[e[0]];
        e[1] = vert_dict[e[1]];
    }

    // update adjacency by merging elements in two keys
    for (const auto& p: vert_dict){
        if (p.first != p.second){
            merge_keys_vectors(adjacency, p.first, p.second);
        }
    }
}


// Method to set the value of a vertex
void GGraph::add_vertex(int v, GradePoint grade) {
    vertices.emplace_back(v);
    vert_grades[v] = grade;
}

// Method to print the graph
void GGraph::print_adjacency() const {
    std::cout << "Print Adjacency" << std::endl;
    // Iterate and print the adjacency list map
    for (const auto& pair : adjacency) {
        int v = pair.first; // vertex
        std::cout << "v = " << v <<": ";
        for (const auto& eid : pair.second) {
            std::cout << get_edge(eid) <<" ";
        }
        std::cout << std::endl;
    }
}


// Method to print the stack
void GGraph::printStack(const std::stack<Vertex>& stack) const {
    std::stack<Vertex> tempStack = stack;
    std::vector<Vertex> elements;
    while (!tempStack.empty()) {
        elements.push_back(tempStack.top());
        tempStack.pop();
    }
    std::reverse(elements.begin(), elements.end());
    std::cout << "Current stack: ";
    for (const auto& elem : elements) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

// Method for Depth-First Search
void GGraph::DFS(Vertex startVertex) const {
    std::unordered_map<Vertex, bool> visited;
    for (Vertex vertex : vertices) {
        visited[vertex] = false;
    }
    std::stack<Vertex> stack;

    // Push the starting vertex onto the stack
    stack.push(startVertex);

    while (!stack.empty()) {
        Vertex vertex = stack.top();
        stack.pop();

        // If the vertex has not been visited, mark it as visited and process it
        if (!visited[vertex]) {
            std::cout << "Visited " << vertex << std::endl;
            visited[vertex] = true;

            // Get all adjacent vertices of the popped vertex
            // If an adjacent vertex has not been visited, push it onto the stack
            for (const auto& eid : adjacency.at(vertex)) {
                Edge e = get_edge(eid);
                Vertex u = (e[0] == vertex) ? e[1]: e[0];
                if (!visited[u]) {
                    stack.push(u);
                }
            }
        }

        // Print the stack after visiting a node
        printStack(stack);
    }
}
