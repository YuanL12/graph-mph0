/*
Still underdeveloped, can be used to break ties of input filtration values.
*/

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

template<typename FT>
class FilGraph {// filtered Graph
public:
    // using VEdges = std::unordered_map<Edge, std::vector<FT>, boost::hash<std::pair<int, int>>>;
    using VAdj = std::vector<EdgeId>; // single vertex adjacency
    using SimplexID = size_t;

    // Constructor reserve sizes 
    FilGraph(int n);

    // Copy Constructor
    FilGraph(const FilGraph& other);

    // vector constructor
    FilGraph(const std::vector<int>& node_labels, 
          const std::vector<FT>& node_features, 
          const std::vector<std::pair<int, int>>& edges_input, 
          const std::vector<FT>& edge_features) {
        
        // Input shape check 
        assert(node_labels.size() == node_features.size() &&"node_labels.size()!= node_features.size()");
        assert(node_labels.size() == node_features.size() &&"node_labels.size()!= node_features.size()");
        
        // IC(node_features);
        // IC(edges_input);
        filtered_simplices.reserve(node_labels.size()+edges_input.size());

        // veritcies first
        size_t dim = 0;
        for (size_t i = 0; i < node_labels.size(); ++i) {
            filtered_simplices.emplace_back(std::make_tuple(dim, i, node_features[i]));
            vertices_labels.emplace_back(v);
        }
        vertex_id_assign = node_labels.size();

        // edges second
        dim = 1;
        for (size_t i = 0; i < edges_input.size(); ++i) {
            filtered_simplices.emplace_back(std::make_tuple(1, i, edge_features[i]));
        }
        edge_id_assign = edges_input.size();
    }

    // get # of vertices
    inline int get_nvertices() const {return vertices.size();};

    //  get all vertices
    inline const std::vector<int>& get_vertices() const {return vertices;};

    //  get adjacency list at vertex v
    VAdj get_adj(Vertex v) const;

    //  get edge by its id
    Edge get_edge(EdgeId id) const;

    //  get edge value 
    inline FT get_edge_value(EdgeId i) const {return edge_values.at(i);};

    // get the filtration value of a single vertex
    FT get_vertex_value(int v) const;

    // Method to add an edge to the FilGraph
    void add_edge(int v, int w, FT value);
    
    // Overloaded method to add an edge with default value 0.0
    void add_edge(int v, int w);

    // Method to set the value of a vertex
    void add_vertex(int v, FT value);

    // Method to print the FilGraph
    void print_adjacency() const;

    // Method to print the FilGraph
    void print_filtration_value() const;
    
    // Remove edge e
    void remove_edge(Edge e);
    
    // Remove vertices based on dictionary
    void remove_vertices(std::unordered_map<int, int> vert_dict);

    // Update FilGraph based on dictionary
    void update_graph(std::unordered_map<int, int> vert_dict);

private:
    // an unique identity can be assigned to the next edge 
    SimplexID edge_id_assign = 0;
    // an unique identity can be assigned to the next vertex 
    SimplexID vertex_id_assign = 0; 

    // a vector of filtered simplices stored as dimension, index(ID), its filtration value
    std::vector<std::tuple<size_t, size_t, FT>> filtered_simplices;
    
    // a vector that maps a vertex ID to a vertex
    std::vector<Vertex> vertices_labels;

    // a vector that maps an edge ID to an edge
    std::vector<SimplexID> Edges;

    // Adjacency list to represent the graph, map vertex to edge Id  
    std::unordered_map<Vertex, VAdj> adjacency;
    

    void security_check_adjacency_map(){
        for (const auto& vertex : vertices) {
            // Check if the vertex is in the adjacency_map
            if (adjacency.find(vertex) == adjacency.end()) {
                // If not, add the vertex with an empty vector as the value
                adjacency[vertex] = std::vector<EdgeId>();
            }
        }
    }

    void security_check_shapes(){
        assert(vertices.size() == vert_values.size() && "vertices.size() !=  vert_values.size()");
        assert(vertices.size() == adjacency.size() && "vertices.size() !=  adjacency.size()");
        assert(edges.size() == edge_values.size() && "edges.size() !=  edge_values.size()");
    }
};





// Constructor
template<typename FT>
FilGraph<FT>::FilGraph(int n) {vertices.reserve(n);}

// Copy Constructor
template<typename FT>
FilGraph<FT>::FilGraph(const FilGraph& other) 
    : edge_id_assign(other.edge_id_assign),
      vert_values(other.vert_values),
      vertices(other.vertices),
      edges(other.edges), 
      edge_values(other.edge_values), 
      adjacency(other.adjacency)
{security_check_shapes();}

// Method to add an edge to the FilGraph
template<typename FT>
void FilGraph<FT>::add_edge(int v, int w, FT value) {
    if (v > w) {
        std::swap(v, w);
    }
    edge_id_assign++;
    Edge edge = Edge(v, w, edge_id_assign);
    edges[edge_id_assign] = edge;
    edge_values[edge_id_assign] = value;
    adjacency[v].emplace_back(edge_id_assign);
    adjacency[w].emplace_back(edge_id_assign);
    security_check_adjacency_map();
}

// Overloaded method to add an edge with default value 0.0
template<typename FT>
void FilGraph<FT>::add_edge(int v, int w) {
    add_edge(v, w, FT());
}

template<typename FT>
FT FilGraph<FT>::get_vertex_value(Vertex v) const {
    auto it = vert_values.find(v);
    if(it == vert_values.end()){
        std::cout << "vertex not found when trying to get its filtration value" << std::endl;
    }
    assert(it != vert_values.end());
    return vert_values.at(v);
}

template<typename FT>
typename FilGraph<FT>::VAdj FilGraph<FT>::get_adj(Vertex v) const {
    auto it = adjacency.find(v);
    if(it == adjacency.end()){
        std::cout << "vertex not found when trying to get its adjacency list" << std::endl;
    }
    assert(it != adjacency.end());
    return adjacency.at(v);
}

template<typename FT>
Edge FilGraph<FT>::get_edge(EdgeId id) const {
    auto it = edges.find(id);
    if(it == edges.end()){
        std::cout << "id not found when trying to get the edge" << std::endl;
    }
    assert(it != edges.end());
    return edges.at(id);
}
    

// Remove edge e 
template<typename FT>
void FilGraph<FT>::remove_edge(Edge e){
    Vertex v0 = e.get_v0();
    Vertex v1 = e.get_v1();
    EdgeId id = e.get_id();
    // Check if the edge exists in edges
    if (edges.find(id) != edges.end()) { // remove it
        edges.erase(id);
        edge_values.erase(id);
        remove_from_adjacency(v0, id);
        remove_from_adjacency(v1, id);
    } else {
        std::cout << "Edge " << e << " does not exist." << std::endl;
    }
}


template<typename FT> // unsafe b/c we don't check if edge use the vertex to be removed 
void FilGraph<FT>::remove_vertices(std::unordered_map<int, int> vert_dict){
    std::vector<int> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto& p: vert_dict){
        if (p.first == p.second){
            new_vertices.emplace_back(p.first);
        }else{
            adjacency.erase(p.first);
            vert_values.erase(p.first);
        }
    }
    vertices = new_vertices;
}

// merge vector of key1 to the vector of key2 and delete key1 later 
void mergeKeys(std::unordered_map<int, std::vector<size_t>>& map, int key1, int key2) {
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


// unsafe b/c we don't check if edge use the vertex to be removed 
template<typename FT> 
void FilGraph<FT>::update_graph(std::unordered_map<int, int> vert_dict){
    // update vertices
    std::vector<int> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto& p: vert_dict){
        if (p.first == p.second){
            new_vertices.emplace_back(p.first);
        }else{
            vert_values.erase(p.first);
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
            mergeKeys(adjacency, p.first, p.second);
        }
    }

}


// Method to set the value of a vertex
template<typename FT>
void FilGraph<FT>::add_vertex(int v, FT value) {
    vertices_labels.emplace_back(v);
    filtered_simplices.emplace_back(std::make_tuple(0, vertex_id_assign++, value));
}

// Method to print the FilGraph
template<typename FT>
void FilGraph<FT>::print_adjacency() const {
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

// Method to print the graph
template<typename FT>
void FilGraph<FT>::print_filtration_value() const {
    std::cout << "Graph filtration values:" << std::endl;
    std::cout << "Vertices:" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "f(" << v <<") = "<< vert_values.at(v)<<std::endl;
    }
    std::cout << "edge_values:" << std::endl;
    for (const auto& eid_value : edge_values) {
        Edge e = edges.at(eid_value.first);
        std::cout << "e = " << e << ", f(e)= "<< eid_value.second << std::endl;
    }
}
