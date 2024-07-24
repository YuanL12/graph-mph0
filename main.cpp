#include "graph.hpp"

/*
    Algorithm 1: Collapse local collapsible edges
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
std::unordered_map<int, int> localCollapse(const Graph& g1, Graph& g2) {
    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::unordered_map<int, bool> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
        visited[v] = false;
    }
    // Initialize an new edge list
    VEdges new_E = g1.get_edge_values();

    std::tuple<int, double, std::string> myTuple(1, 3.14, "Hello");
    typedef std::tuple<int, Edge, int> VEV; // type of (vertex, edge, vertex)
    
    for (auto v: g1.get_vertices() ) {
        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (!visited[u]) {
                // std::cout << "Visit " << u << std::endl;
                visited[u] = true;
                if (e != NULL_EDGE){
                    vertex_dict[u] = v;
                    g2.remove_edge(e);
                }
                // local collapsible
                for (const auto& pair : g2.get_edge_values()) {
                    Edge e = pair.first;
                    double fe = pair.second;
                    int e0, e1;
                    std::tie(e0, e1) = e;
                    if (e0 == u || e1 == u){
                        int x = (e0 == u) ? e1 : e0;
                        double fx, fu;
                        fx = g2.get_vertex_value(x);
                        fu = g2.get_vertex_value(u);
                        if (fe == fx == fu){
                            stack.push(VEV(v, e, x));
                        }
                    }
                }
            }

            // Print the stack after visiting a node
            // printStack(stack);
        }
    }
    return vertex_dict;
}



int main() {
    Graph g(3); 

    g.add_vertex(0, 1.0);
    g.add_vertex(1, 1.0);
    g.add_vertex(2, 1.0);

    g.add_edge(0, 1, 1.0);
    g.add_edge(0, 2, 1.0);
    g.add_edge(1, 2, 1.0);

    std::cout << "Graph adjacency list representation:" << std::endl;
    g.print_adjacency();
    std::cout << "Graph filtration values:" << std::endl;
    g.print_filtrataion_value();

    std::cout << "\nDepth-First Search starting from vertex 0:" << std::endl;
    g.DFS(0);

    Graph new_g = Graph(g);
    std::unordered_map<int, int> vert_dict = localCollapse(g, new_g);

    std::cout << "Graph adjacency list representation:" << std::endl;
    new_g.print_adjacency();
    std::cout << "Graph filtration values:" << std::endl;
    new_g.print_filtrataion_value();

    std::cout << "Vertex Dictionary" << std::endl;
    for (auto& p: vert_dict){
        std::cout << p.first << " -> " << p.second << std::endl;
    }

    return 0;
}