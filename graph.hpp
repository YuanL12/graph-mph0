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
using Edge = std::pair<int, int>;

const Edge NULL_EDGE = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};

template<typename Poset>
class Graph {
public:

    using VEdges = std::unordered_map<Edge, Poset, boost::hash<std::pair<int, int>>>;

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



// Constructor
template<typename Poset>
Graph<Poset>::Graph(int n): nvertices(n) {vertices.reserve(n);}

// Copy Constructor
template<typename Poset>
Graph<Poset>::Graph(const Graph& other) 
    : vertices(other.vertices),
      nvertices(other.nvertices), 
      adj_list(other.adj_list), 
      vert_values(other.vert_values), 
      edge_values(other.edge_values) 
{}
template<typename Poset>
int Graph<Poset>::get_nvertices() const{ return nvertices;}

// Method to add an edge to the graph
template<typename Poset>
void Graph<Poset>::add_edge(int v1, int v2, double value) {
    int v, w;
    if (v1 <= v2){
        v = v1; w = v2;
    }else{
        v = v2; w = v1;
    }

    adj_list[v].push_back(w);
    adj_list[w].push_back(v); 
    edge_values[std::make_pair(v, w)] = value; // Assign value to the edge
    // edge_values[std::make_pair(w, v)] = value; // For undirected graph, assign value in both directions
}

// Overloaded method to add an edge with default value 0.0
template<typename Poset>
void Graph<Poset>::add_edge(int v, int w) {
    add_edge(v, w, 0.0);
}

template<typename Poset>
double Graph<Poset>::get_vertex_value(int v) const {
    auto it = vert_values.find(v);
    
    if(it == vert_values.end()){
        std::cout << "vertex not found when trying to get its filtration value" << std::endl;
    }
    assert(it != vert_values.end());
    return vert_values.at(v);
}
    

// Remove edge e
template<typename Poset>
void Graph<Poset>::remove_edge(Edge e){
    // Check if the key exists using find
    auto it = edge_values.find(e);
    if (it != edge_values.end()) {
        // Key exists, remove it
        edge_values.erase(it);
        std::cout << "Edge (" << e.first << ", " << e.second << ") was removed." << std::endl;
    } else {
        std::cout << "Edge (" << e.first << ", " << e.second << ") does not exist." << std::endl;
    }
}

template<typename Poset>
void Graph<Poset>::remove_edge(int v1, int v2){
    int v, w;
    if (v1 <= v2){
        v = v1; w = v2;
    }else{
        v = v2; w = v1;
    }
}



// Method to set the value of a vertex
template<typename Poset>
void Graph<Poset>::add_vertex(int v, double value) {
    vertices.emplace_back(v);
    vert_values[v] = value;
}

// Method to print the graph
template<typename Poset>
void Graph<Poset>::print_adjacency() const {
    // Iterate and print the adjacency list map
    for (const auto& pair : adj_list) {
        int v = pair.first; // vertex
        std::cout << "v = " << v <<": ";
        for (int v2 : pair.second) {
            std::cout <<  v2 <<" ";
        }
        std::cout << std::endl;
    }
}

// Method to print the graph
template<typename Poset>
void Graph<Poset>::print_filtrataion_value() const {
    // Iterate and print the adjacency list map
    std::cout << "Vertices:" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "f(" << v <<") = "<< vert_values.at(v)<<std::endl;
    }
    std::cout << "Edges:" << std::endl;
    for (const auto& e : edge_values) {
        auto p = e.first;
        std::cout << "f(" << p.first << ", " << p.second << ") = "<< e.second << std::endl;
    }
}

// Method to print the stack
template<typename Poset>
void Graph<Poset>::printStack(const std::stack<int>& stack) const {
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
template<typename Poset>
void Graph<Poset>::DFS(int startVertex) const {
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
            for (const auto& adjVertex : adj_list.at(vertex)) {
                if (!visited[adjVertex]) {
                    stack.push(adjVertex);
                }
            }
        }

        // Print the stack after visiting a node
        printStack(stack);
    }
}

