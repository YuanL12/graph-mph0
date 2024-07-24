#pragma once


#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <algorithm> // Include this for std::reverse
#include <unordered_map> // Include this for edge values
#include <boost/functional/hash.hpp> // Include this for Boost hash
#include <utility> // Include this for std::pair

// Define valued edge type
typedef std::pair<int, int> Edge;
typedef std::unordered_map<Edge, double, boost::hash<std::pair<int, int>>> VEdges;
const Edge NULL_EDGE = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};


class Graph {
public:
    // Constructor reserve sizes 
    Graph(int n);

    // Copy Constructor
    Graph(const Graph& other);

    // get # of vertices
    int get_nvertices() const;

    //  get all vertices
    inline std::vector<int>  get_vertices() const {return vertices;};

    //  get all edge values 
    inline VEdges get_edge_values() const {return edge_values;};

    // get the filtration value of a single vertex
    double get_vertex_value(int v) const;

    // Method to add an edge to the graph
    void add_edge(int v, int w, double value);
    
    // Overloaded method to add an edge with default value 0.0
    void add_edge(int v, int w);

    // Method to set the value of a vertex
    void add_vertex(int v, double value);

    // Method to print the graph
    void print_adjacency() const;

    // Method to print the graph
    void print_filtrataion_value() const;
    
    // Remove edge e; 
    void remove_edge(Edge e);
    void remove_edge(int v, int w);

    // Method for Depth-First Search
    void DFS(int startVertex) const;

private:
    // TODO: adjacency list should be a hash table, so does vertexValues
    std::vector<int> vertices; // vertices
    int nvertices; // Number of vertices
    std::unordered_map<int, std::list<int>> adj_list; // Adjacency list
    std::unordered_map<int, double> vert_values; // Values for vertices
    VEdges edge_values; // Values for edges

    // Method to print the stack
    void printStack(const std::stack<int>& stack) const;
};