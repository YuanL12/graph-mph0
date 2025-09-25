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

/*
    (Filtered) Graph class
    ------------------------------------------------------------
    Every edge or vertex has its filtration value with type FT
    - can be used for both 1-critical filtration and degree-Rips filtration
    - or you can pass in your own filtration

    Note that vertices are initialized as [0, 1, 2, ..., n-1], but it will change after
   collapse
*/
template <typename FT>
class Graph {
   public:
    // using VEdges = std::unordered_map<Edge, std::vector<FT>, boost::hash<std::pair<int,
    // int>>>;
    using VAdj = std::vector<EdgeId>;  // single vertex adjacency
    // Constructor reserve sizes
    Graph(int n);

    // Copy Constructor
    Graph(const Graph& other);

    // vector constructor
    Graph(const std::vector<int>& node_labels, const std::vector<FT>& node_features,
          const std::vector<std::pair<int, int>>& edges_input,
          const std::vector<FT>& edge_features) {
        // IC(node_features);
        // IC(edges_input);
        // Initialize vertices
        for (size_t i = 0; i < node_labels.size(); ++i) {
            add_vertex(node_labels[i], node_features[i]);
        }

        // Initialize edges
        for (size_t i = 0; i < edges_input.size(); ++i) {
            add_edge(edges_input[i].first, edges_input[i].second, edge_features[i]);
        }
        security_check_adjacency_map();
        security_check_shapes();
    }

    // Point cloud constructor
    // Cautions: the graph constructed is degree-Rips filtration
    // and not graph from the point cloud directly
    template <typename PT>  // PT: Point type
    Graph(const std::vector<std::vector<PT>>& points, bool x_y_swapped = false);

    // print sizes of all data structures
    void print_sizes() const {
        std::cout << "Graph size: " << vertices.size() << std::endl;
        std::cout << "Edge size: " << edges.size() << std::endl;
        std::cout << "Adjacency size: " << adjacency.size() << std::endl;
        std::cout << "Edge values size: " << edge_values.size() << std::endl;
        std::cout << "Vertex values size: " << vert_values.size() << std::endl;
    }

    //  get all vertices
    inline const std::vector<int>& get_vertices() const { return vertices; };

    //  get adjacency list at vertex v
    VAdj get_adj(int v) const;

    //  get edge by its id
    Edge get_edge(EdgeId id) const;

    //  get all edge values
    inline const std::unordered_map<EdgeId, FT>& get_edges_values() const { return edge_values; };

    //  get all vertices values
    inline const std::unordered_map<int, FT>& get_vert_values() const { return vert_values; };

    //  get all vertices values in a vector
    inline std::vector<FT> get_vert_values_vector() const {
        std::vector<FT> vert_values_vector;
        vert_values_vector.reserve(vertices.size());
        for (const auto& v : vertices) {
            vert_values_vector.emplace_back(vert_values.at(v));
        }
        return vert_values_vector;
    };

    //  get edge value
    inline FT get_edge_value(EdgeId i) const { return edge_values.at(i); };

    // get the filtration value of a single vertex
    FT get_vertex_value(int v) const;

    // Method to add an edge to the graph
    void add_edge(int v, int w, FT value);

    // Overloaded method to add an edge with default value 0.0
    void add_edge(int v, int w);

    // Method to add multiple edges to the graph
    void add_edges(const std::vector<std::tuple<int, int, FT>>& fil_edges);

    // Method to set the value of a vertex
    void add_vertex(int v, FT value);

    // Method to print the graph
    void print_adjacency() const;

    // Method to print the graph
    void print_filtration_value() const;

    // Remove edge e
    void remove_edge(Edge e);

    // Remove vertices based on dictionary
    void remove_vertices(std::unordered_map<int, int> vert_dict);

    // Update graph based on dictionary
    void update_graph(std::unordered_map<int, int> vert_dict);

    // Method for Depth-First Search
    void DFS(int startVertex) const;

    // Helper function to remove an edge from the adjacency hash map
    void remove_from_adjacency(int v, EdgeId edgeId) {
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
    EdgeId edge_id_assign = 0;  // used to assign next edge an unique identity
    std::vector<int> vertices;
    std::unordered_map<int, FT> vert_values;
    std::unordered_map<EdgeId, Edge> edges;
    std::unordered_map<EdgeId, FT> edge_values;
    // Adjacency list to represent the graph, map vertex to edge Id
    std::unordered_map<int, VAdj> adjacency;

    // Method to print the stack
    void printStack(const std::stack<int>& stack) const;

    // merge vector of key1 to the vector of key2 and delete key1 later
    template <typename keyT, typename VectT>
    void merge_keys_vectors(std::unordered_map<keyT, std::vector<VectT>>& map, keyT key1,
                            keyT key2) {
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

#ifdef ENABLE_SECURITY_CHECKS
    void security_check_adjacency_map() {
        for (const auto& vertex : vertices) {
            // Check if the vertex is in the adjacency_map
            if (adjacency.find(vertex) == adjacency.end()) {
                // If not, add the vertex with an empty vector as the value
                adjacency[vertex] = std::vector<EdgeId>();
            }
        }
    }
#else
    void security_check_adjacency_map() {
        // do nothing
    }
#endif

#ifdef ENABLE_SECURITY_CHECKS
    void security_check_shapes() {
        assert(vertices.size() == vert_values.size() && "vertices.size() !=  vert_values.size()");
        assert(vertices.size() == adjacency.size() && "vertices.size() !=  adjacency.size()");
        assert(edges.size() == edge_values.size() && "edges.size() !=  edge_values.size()");
    }
#else
    void security_check_shapes() {
        // do nothing
    }
#endif
};

// Constructor
template <typename FT>
Graph<FT>::Graph(int n) {
    vertices.reserve(n);
}

// Copy Constructor
template <typename FT>
Graph<FT>::Graph(const Graph& other)
    : edge_id_assign(other.edge_id_assign),
      vert_values(other.vert_values),
      vertices(other.vertices),
      edges(other.edges),
      edge_values(other.edge_values),
      adjacency(other.adjacency) {
    security_check_shapes();
}

template <typename FT>  // FT: Filtration value type
template <typename PT>  // PT: Point type
Graph<FT>::Graph(const std::vector<std::vector<PT>>& points, bool x_y_swapped) {
    /*
    Convert a point cloud to a 1-critical filtration. The added vertex index follows
    the row-major order of the distance matrix.
    ------------------------------------------------------------
    Args:
        points: list of points in R^d
        x_y_swapped: if true, the x and y coordinates are swapped
    Returns:
        vertices: list of vertices
        edges: list of edges
        filt_func_v: list of vertices in the filtration function
        filt_func_e: list of edges in the filtration function
    */

    // Create vertices
    const int n = points.size();

    // Create vertices
    vertices.resize(n * n);
    std::iota(vertices.begin(), vertices.end(), 0);

    // Reserve space for vert_values
    vert_values.reserve(n * n);

    // Compute distance matrix
    std::vector<int> degrees(n);  // [0, 1, 2, ..., n-1]
    std::vector<std::vector<PT>> D(n, std::vector<PT>(n, PT(0.0)));
    for (int i = 0; i < n; ++i) {
        degrees[i] = i;
        for (int j = i + 1; j < n; ++j) {
            PT dist = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k)
                dist += (points[i][k] - points[j][k]) * (points[i][k] - points[j][k]);
            D[i][j] = std::sqrt(dist);
            D[j][i] = D[i][j];
        }
    }

    // Precompute sorted distances of each row
    std::vector<std::vector<PT>> sorted_dists(n, std::vector<PT>(n));
    for (int i = 0; i < n; ++i) {
        sorted_dists[i] = D[i];
        std::sort(sorted_dists[i].begin(), sorted_dists[i].end());
    }

    // Add vertex values
    std::vector<FT> filt_func_v;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int v = i * n + j;
            PT x = sorted_dists[i][j];
            PT y = -degrees[j];
            if (x_y_swapped)
                vert_values[v] = FT(y, x);
            else
                vert_values[v] = FT(x, y);
        }
    }

    // Add edges from the same row
    std::vector<FT> filt_func_e;
    for (int i = 0; i < n; ++i) {
        int base = i * n;
        // Add edges from the same row e_j = (j, j+1), j = 0, 1, ..., n-2
        // f(e_j) = (r_{j+1} , -j)
        for (int j = 0; j < n - 1; ++j) {
            PT x = sorted_dists[i][j + 1];
            PT y = -degrees[j];
            if (x_y_swapped)
                add_edge(base + j, base + j + 1, FT(y, x));
            else
                add_edge(base + j, base + j + 1, FT(x, y));
        }
    }

    // Add edges across vertices
    for (int i = 0; i < n; ++i) {
        const auto& di = sorted_dists[i];
        for (int j = i + 1; j < n; ++j) {
            const auto& dj = sorted_dists[j];

            // compute max distance between i and j
            std::vector<PT> max_r(n);
            for (int k = 0; k < n; ++k) max_r[k] = std::max(di[k], dj[k]);

            // find the first index k such that max_r[k] >= D[i][j]
            PT threshold = D[i][j];
            auto it = std::lower_bound(max_r.begin(), max_r.end(), threshold);
            int idx = it - max_r.begin();

            // add edges from i to j
            for (int k = idx; k < n; ++k) {
                // transform to 1D vertex index
                int ii = i * n + k;
                int jj = j * n + k;
                PT x = max_r[k];
                PT y = -degrees[k];
                if (x_y_swapped)
                    add_edge(ii, jj, FT(y, x));
                else
                    add_edge(ii, jj, FT(x, y));
            }
        }
    }
}

// Method to add an edge to the graph
template <typename FT>
void Graph<FT>::add_edge(int v, int w, FT value) {
    if (v > w) {
        std::swap(v, w);
    }
    Edge edge = Edge(v, w, edge_id_assign);
    edges[edge_id_assign] = edge;
    edge_values[edge_id_assign] = value;
    adjacency[v].emplace_back(edge_id_assign);
    adjacency[w].emplace_back(edge_id_assign);
    edge_id_assign++;
    security_check_adjacency_map();
}

template <typename FT>
void Graph<FT>::add_edges(const std::vector<std::tuple<int, int, FT>>& fil_edges) {
    this->edges.reserve(this->edges.size() + fil_edges.size());
    // for (const auto& [v, w, val] : fil_edges)
    //     this->edges.emplace_back(v, w, val);
}

// Overloaded method to add an edge with default value 0.0
template <typename FT>
void Graph<FT>::add_edge(int v, int w) {
    add_edge(v, w, FT());
}

template <typename FT>
FT Graph<FT>::get_vertex_value(int v) const {
    auto it = vert_values.find(v);
    if (it == vert_values.end()) {
        std::cout << "vertex not found when trying to get its filtration value" << std::endl;
    }
    assert(it != vert_values.end());
    return it->second;
}

template <typename FT>
typename Graph<FT>::VAdj Graph<FT>::get_adj(int v) const {
    auto it = adjacency.find(v);
    if (it == adjacency.end()) {
        std::cout << "vertex not found when trying to get its adjacency list" << std::endl;
    }
    assert(it != adjacency.end());
    return it->second;
}

template <typename FT>
Edge Graph<FT>::get_edge(EdgeId id) const {
    auto it = edges.find(id);
    if (it == edges.end()) {
        std::cout << "id not found when trying to get the edge" << std::endl;
    }
    assert(it != edges.end());
    return it->second;
}

// Remove edge e
template <typename FT>
void Graph<FT>::remove_edge(Edge e) {
    int v0 = e.get_v0();
    int v1 = e.get_v1();
    EdgeId id = e.get_id();
    // Check if the edge exists in edges
    if (edges.find(id) != edges.end()) {  // remove it
        edges.erase(id);
        edge_values.erase(id);
        remove_from_adjacency(v0, id);
        remove_from_adjacency(v1, id);
    } else {
        std::cout << "Edge " << e << " does not exist." << std::endl;
    }
}

template <typename FT>  // unsafe b/c we don't check if edge use the vertex to be removed
void Graph<FT>::remove_vertices(std::unordered_map<int, int> vert_dict) {
    std::vector<int> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto& p : vert_dict) {
        if (p.first == p.second) {
            new_vertices.emplace_back(p.first);
        } else {
            adjacency.erase(p.first);
            vert_values.erase(p.first);
        }
    }
    vertices = new_vertices;
}

// unsafe b/c we don't check if edge use the vertex to be removed
template <typename FT>
void Graph<FT>::update_graph(std::unordered_map<int, int> vert_dict) {
    // update vertices
    std::vector<int> new_vertices;
    new_vertices.reserve(vertices.size());
    for (const auto& p : vert_dict) {
        if (p.first == p.second) {
            new_vertices.emplace_back(p.first);
        } else {
            vert_values.erase(p.first);
        }
    }
    vertices = new_vertices;

    // update edges
    for (auto& p : edges) {
        Edge& e = p.second;
        e[0] = vert_dict[e[0]];
        e[1] = vert_dict[e[1]];
    }

    // update adjacency by merging elements in two keys
    for (const auto& p : vert_dict) {
        if (p.first != p.second) {
            merge_keys_vectors(adjacency, p.first, p.second);
        }
    }
}

// Method to set the value of a vertex
template <typename FT>
void Graph<FT>::add_vertex(int v, FT value) {
    vertices.emplace_back(v);
    vert_values[v] = value;
}

// Method to print the graph
template <typename FT>
void Graph<FT>::print_adjacency() const {
    std::cout << "Print Adjacency" << std::endl;
    // Iterate and print the adjacency list map
    for (const auto& pair : adjacency) {
        int v = pair.first;  // vertex
        std::cout << "v = " << v << ": ";
        for (const auto& eid : pair.second) {
            std::cout << get_edge(eid) << " ";
        }
        std::cout << std::endl;
    }
}

// Method to print the graph
template <typename FT>
void Graph<FT>::print_filtration_value() const {
    std::cout << "Graph filtration values:" << std::endl;
    std::cout << "Vertices:" << std::endl;
    for (const auto& v : vertices) {
        std::cout << "f(" << v << ") = " << vert_values.at(v) << std::endl;
    }
    std::cout << "edge_values:" << std::endl;
    for (const auto& eid_value : edge_values) {
        Edge e = edges.at(eid_value.first);
        std::cout << "e = " << e << ", f(e)= " << eid_value.second << std::endl;
    }
}

// Method to print the stack
template <typename FT>
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
template <typename FT>
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
            for (const auto& eid : adjacency.at(vertex)) {
                Edge e = get_edge(eid);
                int u = (e[0] == vertex) ? e[1] : e[0];
                if (!visited[u]) {
                    stack.push(u);
                }
            }
        }

        // Print the stack after visiting a node
        printStack(stack);
    }
}
