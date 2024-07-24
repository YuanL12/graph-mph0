#include "graph.hpp"

// Constructor
Graph::Graph(int n): nvertices(n) {vertices.reserve(n);}

// Copy Constructor
Graph::Graph(const Graph& other) 
    : vertices(other.vertices),
      nvertices(other.nvertices), 
      adj_list(other.adj_list), 
      vert_values(other.vert_values), 
      edge_values(other.edge_values) 
{}

int Graph::get_nvertices() const{ return nvertices;}

// Method to add an edge to the graph
void Graph::add_edge(int v1, int v2, double value) {
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
void Graph::add_edge(int v, int w) {
    add_edge(v, w, 0.0);
}

double Graph::get_vertex_value(int v) const {
    auto it = vert_values.find(v);
    
    if(it == vert_values.end()){
        std::cout << "vertex not found when trying to get its filtration value" << std::endl;
    }
    assert(it != vert_values.end());
    return vert_values.at(v);
}
    

// Remove edge e
void Graph::remove_edge(Edge e){
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

void Graph::remove_edge(int v1, int v2){
    int v, w;
    if (v1 <= v2){
        v = v1; w = v2;
    }else{
        v = v2; w = v1;
    }
}



// Method to set the value of a vertex
void Graph::add_vertex(int v, double value) {
    vertices.emplace_back(v);
    vert_values[v] = value;
}

// Method to print the graph
void Graph::print_adjacency() const {
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
void Graph::print_filtrataion_value() const {
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
void Graph::printStack(const std::stack<int>& stack) const {
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
void Graph::DFS(int startVertex) const {
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

