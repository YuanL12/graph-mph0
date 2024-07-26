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

template<typename FT>
class Graph {
public:

    // using VEdges = std::unordered_map<Edge, std::vector<FT>, boost::hash<std::pair<int, int>>>;
    using VAdj = std::vector<std::pair<Vertex, EdgeId>>; // single vertex adjacency
    // Constructor reserve sizes 
    Graph(int n);

    // Copy Constructor
    Graph(const Graph& other);

    // get # of vertices
    inline int get_nvertices() const {return vertices.size();};

    //  get all vertices
    inline std::vector<int>  get_vertices() const {return vertices;};

    //  get adjacency list at vertex v
    inline VAdj get_adj(Vertex v) const {return adjacency.at(v);};

    //  get edge by its id
    inline Edge get_edge(EdgeId id) const {return edges.at(id);};

    //  get all edge values 
    inline std::unordered_map<EdgeId, FT> get_edges_values() const {return edge_values;};

    //  get edge value 
    inline FT get_edge_values(EdgeId i) const {return edge_values.at(i);};

    // get the filtration value of a single vertex
    FT get_vertex_value(int v) const;

    // Method to add an edge to the graph
    void add_edge(int v, int w, FT value);
    
    // Overloaded method to add an edge with default value 0.0
    void add_edge(int v, int w);

    // Method to set the value of a vertex
    void add_vertex(int v, FT value);

    // Method to print the graph
    void print_adjacency() const;

    // Method to print the graph
    void print_filtrataion_value() const;
    
    // Remove edge e
    void remove_edge(Edge e);
    
    // Remove vertices based on dictionary
    void remove_vertices(std::unordered_map<int, int> vert_dict);

    // Method for Depth-First Search
    void DFS(int startVertex) const;

    // Helper function to remove an edge from the adjacency hash map
    void remove_from_adjacency(Vertex v, EdgeId edgeId) {
        if (adjacency.find(v) != adjacency.end()) {
            auto& neighbors = adjacency[v];
            neighbors.erase(
                std::remove_if(neighbors.begin(), neighbors.end(),
                    [edgeId](const std::tuple<Vertex, EdgeId>& neighbor) {
                        return std::get<1>(neighbor) == edgeId;
                    }),
                neighbors.end()
            );
        }
    }

private:
    EdgeId edge_id_assign = 0; 
    std::vector<Vertex> vertices; //
    std::unordered_map<Vertex, FT> vert_values;
    std::unordered_map<EdgeId, Edge> edges;
    std::unordered_map<EdgeId, FT> edge_values; 
    // Adjacency list to represent the graph, map vertex to edge Id  
    std::unordered_map<Vertex, VAdj> adjacency;
    
    // Method to print the stack
    void printStack(const std::stack<int>& stack) const;
};





// Constructor
template<typename FT>
Graph<FT>::Graph(int n) {vertices.reserve(n);}

// Copy Constructor
template<typename FT>
Graph<FT>::Graph(const Graph& other) 
    : vertices(other.vertices),
      edges(other.edges), 
      edge_values(other.edge_values), 
      adjacency(other.adjacency)
{}

// Method to add an edge to the graph
template<typename FT>
void Graph<FT>::add_edge(int v, int w, FT value) {
    if (v > w) {
        std::swap(v, w);
    }
    edge_id_assign++;
    Edge edge = Edge(v, w, edge_id_assign);
    edges[edge_id_assign] = edge;
    edge_values[edge_id_assign] = value;
    adjacency[v].emplace_back(w, edge_id_assign);
    adjacency[w].emplace_back(v, edge_id_assign);
}

// Overloaded method to add an edge with default value 0.0
template<typename FT>
void Graph<FT>::add_edge(int v, int w) {
    add_edge(v, w, 0.0);
}

template<typename FT>
FT Graph<FT>::get_vertex_value(int v) const {
    auto it = vert_values.find(v);
    if(it == vert_values.end()){
        std::cout << "vertex not found when trying to get its filtration value" << std::endl;
    }
    assert(it != vert_values.end());
    return vert_values.at(v);
}
    

// Remove edge e 
template<typename FT>
void Graph<FT>::remove_edge(Edge e){
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
void Graph<FT>::remove_vertices(std::unordered_map<int, int> vert_dict){
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



// Method to set the value of a vertex
template<typename FT>
void Graph<FT>::add_vertex(int v, FT value) {
    vertices.emplace_back(v);
    vert_values[v] = value;
}

// Method to print the graph
template<typename FT>
void Graph<FT>::print_adjacency() const {
    std::cout << "Print Adjacency" << std::endl;
    // Iterate and print the adjacency list map
    for (const auto& pair : adjacency) {
        int v = pair.first; // vertex
        std::cout << "v = " << v <<": ";
        for (const auto& neighbor : pair.second) {
            std::cout << neighbor.first <<" ";
        }
        std::cout << std::endl;
    }
}

// Method to print the graph
template<typename FT>
void Graph<FT>::print_filtrataion_value() const {
    // Iterate and print the adjacency list map
    std::cout << "Vertices:" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "f(" << v <<") = "<< vert_values.at(v)<<std::endl;
    }
    std::cout << "Edges:" << std::endl;
    for (const auto& eid_value : edge_values) {
        Edge e = edges.at(eid_value.first);
        std::cout << "e = " << e << ", f(e)= "<< eid_value.second << std::endl;
    }
}

// Method to print the stack
template<typename FT>
void Graph<FT>::printStack(const std::stack<int>& stack) const {
    std::stack<int> tempStack = stack;
    std::vector<int> elements;
    while (!tempStack.empty()) {
        elements.push_back(tempStack.top());
        tempStack.pop();
    }
    std::reverse(elements.begin(), elements.end());
    std::cout << "Current stack: ";
    for (const int& elem : elements) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
}

// Method for Depth-First Search
template<typename FT>
void Graph<FT>::DFS(int startVertex) const {
    std::unordered_map<int, bool> visited;
    for (int vertex : vertices) {
        visited[vertex] = false;
    }
    std::stack<int> stack;

    // Push the starting vertex onto the stack
    stack.push(startVertex);

    while (!stack.empty()) {
        int vertex = stack.top();
        stack.pop();

        // If the vertex has not been visited, mark it as visited and process it
        if (!visited[vertex]) {
            std::cout << "Visited " << vertex << std::endl;
            visited[vertex] = true;

            // Get all adjacent vertices of the popped vertex
            // If an adjacent vertex has not been visited, push it onto the stack
            for (const auto& nbrs : adjacency.at(vertex)) {
                if (!visited[nbrs.first]) {
                    stack.push(nbrs.first);
                }
            }
        }

        // Print the stack after visiting a node
        printStack(stack);
    }
}

