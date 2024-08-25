#include "MPH.hpp"

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
    g.print_filtration_value();

    std::cout << "\nDepth-First Search starting from vertex 0:" << std::endl;
    g.DFS(0);

    Graph<double> new_g = Graph<double>(g);
    std::cout << "Collapse locally" << std::endl;
    localCollapse(g, new_g);

    std::cout << "Graph adjacency list representation:" << std::endl;
    new_g.print_adjacency();
    std::cout << "Graph filtration values:" << std::endl;
    new_g.print_filtration_value();
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
    g.print_filtration_value();

    // std::cout << "Collapse locally now" << std::endl;
    // Graph<R2> g2 = Graph<R2>(g); 
    // localCollapse(g, g2);

    std::cout << "Collapse the graph to vertex minimal" << std::endl;
    Graph<R2> g3 = collapse_to_vertex_minimal(g);

    // std::cout << "Graph adjacency list representation:" << std::endl;
    // g3.print_adjacency();

}


int test_figure1_dendrogram() {
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
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_Dengrogram<R2>(g);
    
    std::cout << "Final Results: " << std::endl;
    std::cout << "betti_0:" << std::endl;
    for (const auto& val : betti_0) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_1:" << std::endl;
    for (const auto& val : betti_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_2:" << std::endl;
    for (const auto& val : betti_2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_0_1:" << std::endl;
    for (const auto& val : betti_0_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    return 0;
}


int test_figure1_DTree() {
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
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
    
    std::cout << "Final Results: " << std::endl;
    std::cout << "betti_0:" << std::endl;
    for (const auto& val : betti_0) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_1:" << std::endl;
    for (const auto& val : betti_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_2:" << std::endl;
    for (const auto& val : betti_2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_0_1:" << std::endl;
    for (const auto& val : betti_0_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    return 0;
}


int test_3() {
    Graph<R2> g(2); 
    g.add_vertex(0, R2(1,2)); // x1
    g.add_vertex(1, R2(3,4)); // x2

    g.add_edge(0, 1, R2(5,6)); // d1 = (x1, x2)
    g.add_edge(0, 1, R2(7,8)); // d2 = (x2,x3)
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
    
    std::cout << "Final Results: " << std::endl;
    std::cout << "betti_0:" << std::endl;
    for (const auto& val : betti_0) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_1:" << std::endl;
    for (const auto& val : betti_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_2:" << std::endl;
    for (const auto& val : betti_2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "betti_0_1:" << std::endl;
    for (const auto& val : betti_0_1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    return 0;
}

int main() {
    test_3();
    return 0;
}