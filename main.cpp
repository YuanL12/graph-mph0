#include "Graph.hpp"
#include "Poset.hpp"
#include <set>
/*
    Algorithm 1: Collapse local collapsible edges
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
template<typename FT>
void localCollapse(const Graph<FT>& g1, Graph<FT>& g2) {
    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::unordered_map<int, bool> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
        visited[v] = false;
    }

    // type of (vertex, edge, vertex)
    using VEV = std::tuple<int, Edge, int>; 
    // Run depth-first search from v on local edges 
    for (auto v: g1.get_vertices() ) {
        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
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
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g2.remove_edge(e);
                    std::cout << "Remove "<< e << std::endl;
                }
                // local collapsible
                for (const auto& eid_local: g1.get_adj(u)){
                    e = g1.get_edge(eid_local);
                    Vertex x = (e[0] == u) ? e[1]: e[0];
                    
                    FT fx, fu, fe;
                    fx = g1.get_vertex_value(x);
                    fu = g1.get_vertex_value(u);
                    fe = g1.get_edge_value(eid_local);
                    if (fe == fx && fx == fu){
                        stack.push(VEV(v, g1.get_edge(eid_local), x));
                    }
                }
            }
        }
    }
    // update vertices and edges
    g2.update_graph(vertex_dict);
}


/*
    Algorithm 2: Collapse to vertex-minimal graph
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
template<typename FT>
Graph<FT> collapse_to_vertex_minimal(const Graph<FT>& g) {
    Graph<R2> g1 = Graph<R2>(g);
    localCollapse(g, g1);
    Graph<R2> g2 = Graph<R2>(g1); 

    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::set<int> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
    }

    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<int, Edge, int>;
    // Run depth-first search from minimal vertices
    for (auto v: g1.get_vertices() ) {
        FT fv = g1.get_vertex_value(v);
        bool check_minimal = true;
        for (const auto& eid : g1.get_adj(v)) {
            Edge e = g1.get_edge(eid);
            Vertex u = (e[0] == v) ? e[1]: e[0];
            FT fe = g1.get_edge_value(eid);
            FT fu = g1.get_vertex_value(u);
            if (fe == fv && fe > fu){ // v is not minimal
                check_minimal = false;
                break;
            }
        }
        if(!check_minimal){continue;}

        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (visited.find(u) == visited.end()) {
                // std::cout << "Visit " << u << std::endl;
                visited.insert(u);
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g2.remove_edge(e);
                    std::cout << "Remove "<< e << std::endl;
                }


                // local collapsible
                for (const auto& eid_local: g1.get_adj(u)){
                    e = g1.get_edge(eid_local);
                    Vertex x = (e[0] == u) ? e[1]: e[0];
                    FT fx, fu, fe;
                    fx = g1.get_vertex_value(x);
                    fu = g1.get_vertex_value(u);
                    fe = g1.get_edge_value(eid_local);
                    if (fe == fx && fx > fu){
                        stack.push(VEV(v, g1.get_edge(eid_local), x));
                    }
                }
            }
        }
    }

    // update vertices 
    g2.update_graph(vertex_dict);
    g2.print_filtrataion_value();
    return g2;
}



void test1(){
    Graph<double> g(3); 

    g.add_vertex(0, 1.0);
    g.add_vertex(1, 1.0);
    g.add_vertex(2, 1.0);

    g.add_edge(0, 1, 1.0);
    g.add_edge(0, 2, 1.0);
    g.add_edge(1, 2, 1.0);

    std::cout << "Graph adjacency list representation:" << std::endl;
    g.print_adjacency();
    g.print_filtrataion_value();

    std::cout << "\nDepth-First Search starting from vertex 0:" << std::endl;
    g.DFS(0);

    Graph<double> new_g = Graph<double>(g);
    std::cout << "Collapse locally" << std::endl;
    localCollapse(g, new_g);

    std::cout << "Graph adjacency list representation:" << std::endl;
    new_g.print_adjacency();
    std::cout << "Graph filtration values:" << std::endl;
    new_g.print_filtrataion_value();
}



void test2(){
    Graph<R2> g(6); 
    g.add_vertex(1, R2(6,2)); // x1
    g.add_vertex(2, R2(6,2)); // x2
    g.add_vertex(3, R2(6,2)); // x3
    g.add_vertex(4, R2(1,3)); // u
    g.add_vertex(5, R2(2,1)); // v 
    g.add_vertex(6, R2(2,6)); // w

    g.add_edge(1, 2, R2(6,2)); // d1 = (x1, x2)
    g.add_edge(1, 3, R2(6,2)); // d2 = (x2,x3)
    g.add_edge(2, 3, R2(6,2)); // d3 = (x1,x3)
    g.add_edge(4, 5, R2(3,5)); // e1 = (u,v)
    g.add_edge(4, 5, R2(5,3)); // e2 = (u,v)
    g.add_edge(5, 1, R2(6,2)); // h1 = (v, x1)
    g.add_edge(4, 6, R2(2,6)); // h2 = (u, w)
    g.add_edge(6, 3, R2(6,6)); // e3 = (w, x3)

    std::cout << "Graph adjacency list representation:" << std::endl;
    g.print_adjacency();
    g.print_filtrataion_value();

    // std::cout << "Collapse locally now" << std::endl;
    // Graph<R2> g2 = Graph<R2>(g); 
    // localCollapse(g, g2);

    std::cout << "Collapse the graph to vertex minimal" << std::endl;
    Graph<R2> g3 = collapse_to_vertex_minimal(g);

    // std::cout << "Graph adjacency list representation:" << std::endl;
    // g3.print_adjacency();

}


int main() {
    test2();

    return 0;
}