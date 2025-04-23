#include "MPH.hpp"
#include <icecream.hpp>
#include "IO.hpp"
#include "PointCloud.hpp"

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
    std::cout << "Collapse the graph to vertex minimal" << std::endl;
    Graph<R2> g3 = collapse_to_vertex_minimal(g);

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
    IC(betti_0, betti_1, betti_2, betti_0_1);
    /*
    Expect Final Results: 
        betti_0: (1, 3) (2, 1) 
        betti_1: (3, 5) (5, 3) 
        betti_2: (5, 5) 
        betti_0_1: (5, 5) (6, 2) (6, 6) 
    */
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
    IC(betti_0, betti_1, betti_2, betti_0_1);

    /*
    Expect Final Results: 
        betti_0: (1, 3) (2, 1) 
        betti_1: (3, 5) (5, 3) 
        betti_2: (5, 5) 
        betti_0_1: (5, 5) (6, 2) (6, 6) 
    */
    return 0;
}


int test_figure1_GGraph() {
    GGraph g(6); 
    g.add_vertex(1, GradePoint(6,2)); // x1
    g.add_vertex(2, GradePoint(6,2)); // x2
    g.add_vertex(3, GradePoint(6,2)); // x3
    g.add_vertex(4, GradePoint(1,3)); // u
    g.add_vertex(5, GradePoint(2,1)); // v 
    g.add_vertex(6, GradePoint(2,6)); // w

    g.add_edge(1, 2, GradePoint(6,2)); // d1 = (x1, x2)
    g.add_edge(1, 3, GradePoint(6,2)); // d2 = (x2,x3)
    g.add_edge(2, 3, GradePoint(6,2)); // d3 = (x1,x3)
    g.add_edge(4, 5, GradePoint(3,5)); // e1 = (u,v)
    g.add_edge(4, 5, GradePoint(5,3)); // e2 = (u,v)
    g.add_edge(5, 1, GradePoint(6,2)); // h1 = (v, x1)
    g.add_edge(4, 6, GradePoint(2,6)); // h2 = (u, w)
    g.add_edge(6, 3, GradePoint(6,6)); // e3 = (w, x3)
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree_Grade_Version(g);
    
    std::cout << "Final Results: " << std::endl;
    IC(betti_0, betti_1, betti_2, betti_0_1);

    /*
    Expect Final Results: 
        betti_0: (1, 3) (2, 1) 
        betti_1: (3, 5) (5, 3) 
        betti_2: (5, 5) 
        betti_0_1: (5, 5) (6, 2) (6, 6) 
    */
    return 0;
}


int test_3() {
    Graph<R2> g(2); 
    g.add_vertex(0, R2(1,2)); // x1
    g.add_vertex(1, R2(3,4)); // x2

    g.add_edge(0, 1, R2(5,6)); 
    g.add_edge(0, 1, R2(7,8)); 
    std::cout << "Input Graph: " << std::endl;
    g.print_adjacency();
    g.print_filtration_value();
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
    
    std::cout << "Final Results: " << std::endl;
    IC(betti_0, betti_1, betti_2, betti_0_1);
    /*
    Expected Result:
    betti_0: [(1, 2), (3, 4)]
    betti_1: [(5, 6)]
    betti_2: []
    betti_0_1: [(7, 8)]
    */
    return 0;
}

void test_4() {
    Graph<R2> g(3); 
    g.add_vertex(1, R2(0,0)); // x1
    g.add_vertex(2, R2(0,0)); // x2
    g.add_vertex(3, R2(0,0)); // x3

    g.add_edge(1, 2, R2(1,4)); 
    g.add_edge(1, 2, R2(4,1)); 
    g.add_edge(2, 3, R2(3,3)); 
    g.add_edge(1, 3, R2(2,2)); 
    std::cout << "Input Graph: " << std::endl;
    g.print_adjacency();
    g.print_filtration_value();
    {
    std::cout << "Dengrogram:" << std::endl;
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_Dengrogram<R2>(g);
    std::cout << "Final Results: " << std::endl;
    IC(betti_0, betti_1, betti_2, betti_0_1);
    std::cout << "\n";
    }
    {
    std::cout << "DTree" << std::endl;
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
    std::cout << "Final Results: " << std::endl;
    IC(betti_0, betti_1, betti_2, betti_0_1);
    }
}


int test_5() {
    Graph<R2> g(2); 
    g.add_vertex(0, R2(1,2)); // x1
    g.add_vertex(1, R2(3,4)); // x2

    g.add_edge(0, 1, R2(7.2,5.8)); 
    g.add_edge(0, 1, R2(9.8,5.9)); 
    std::cout << "Input Graph: " << std::endl;
    g.print_adjacency();
    g.print_filtration_value();
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g, true);
    
    std::cout << "Final Results: " << std::endl;
    IC(betti_0, betti_1, betti_2, betti_0_1);
    /*
    Expected Result:
    betti_0: [(1, 2), (3, 4)]
    betti_1: [(5, 6)]
    betti_2: []
    betti_0_1: [(7, 8)]
    */
    return 0;
}

int test_read_filtration_data(std::string file_name) {
    auto [nV, Es, F_Vs, F_Es] = read_filtration_data<double>(file_name);

    std::cout << "nV: " << nV << std::endl;
    std::cout << "Es: " << Es.size() << std::endl;
    std::cout << "F_Vs: " << F_Vs.size() << std::endl;
    std::cout << "F_Es: " << F_Es.size() << std::endl;
    
    // start building the graph
    Graph<R2> g(nV);
    for (int i = 0; i < nV; i++) {
        g.add_vertex(i, R2(F_Vs[i].first, F_Vs[i].second));
    }
    for (int i = 0; i < Es.size(); i++) {
        g.add_edge(Es[i].first, Es[i].second, R2(F_Es[i].first, F_Es[i].second));
    }
    // time the computation
    auto start = std::chrono::high_resolution_clock::now();
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken to compute MPH0: " << duration.count() << " seconds" << std::endl;

    // std::cout << "Print Final Results: " << std::endl;
    // IC(betti_0, betti_1, betti_2, betti_0_1);
    return 0;
}


int test_read_points(std::string file_name) {
    auto points = read_points<double>(file_name);

    auto time_start = std::chrono::high_resolution_clock::now();
    Graph<R2> g(points);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_duration = time_end - time_start;
    std::cout << "Time taken to build the graph: " << time_duration.count() << " seconds" << std::endl;

    // time the computation
    std::cout << "Computing MPH0..." << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree_Inplace<R2>(g);
    time_end = std::chrono::high_resolution_clock::now();
    time_duration = time_end - time_start;
    std::cout << "Time taken to compute MPH0: " << time_duration.count() << " seconds" << std::endl;
    std::cout << "Betti Numbers sizes: " << std::endl;
    IC(betti_0.size(), betti_1.size(), betti_2.size(), betti_0_1.size());
    if (points.size() <= 5) {
        std::cout << "Final Results: " << std::endl;
        IC(betti_0, betti_1, betti_2, betti_0_1);
    }
    
    return 0;
}



int test_read_points_GradeTable(std::string file_name) {
    // read the file
    auto points = read_points<double>(file_name);

    // build the graph
    auto time_start = std::chrono::high_resolution_clock::now();
    auto [ggraph, grade_table] = point_cloud_to_degree_Rips_filtration<double>(points);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_duration = time_end - time_start;
    std::cout << "Time taken to build the graph and grade table: " << time_duration.count() << " seconds" << std::endl;

    // time the computation
    std::cout << "Computing MPH0..." << std::endl;
    time_start = std::chrono::high_resolution_clock::now();
    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0_DTree_Grade_Version(ggraph);
    time_end = std::chrono::high_resolution_clock::now();
    time_duration = time_end - time_start;
    std::cout << "Time taken to compute MPH0: " << time_duration.count() << " seconds" << std::endl;
    
    // std::cout << "Raw Betti Numbers sizes: " << std::endl;
    // IC(raw_betti_0.size(), raw_betti_1.size(), raw_betti_2.size(), raw_betti_0_1.size());

    // bool x_y_swap = true;
    // if (points.size() <= 10) {
    //     std::cout << "Final Results: " << std::endl;
    //     grade_table.print(x_y_swap);
    //     if (x_y_swap) {
    //         for (auto& b0: raw_betti_0) {
    //             std::swap(b0.first, b0.second); 
    //         }
    //         for (auto& b1: raw_betti_1) {
    //             std::swap(b1.first, b1.second); 
    //         }
    //         for (auto& b2: raw_betti_2) {
    //             std::swap(b2.first, b2.second); 
    //         }
    //         for (auto& b01: raw_betti_0_1) {
    //             std::swap(b01.first, b01.second); 
    //         }
            
    //     }
        
    //     auto betti_0 = sort_count_betti_result(raw_betti_0);
    //     auto betti_1 = sort_count_betti_result(raw_betti_1);
    //     auto betti_2 = sort_count_betti_result(raw_betti_2);
    //     auto betti_0_1 = sort_count_betti_result(raw_betti_0_1);
    //     IC(betti_0, betti_1, betti_2, betti_0_1);
    // }
    
    return 0;
}

int main(int argc, char** argv) {
    // test_figure1_dendrogram();
    // test_3();
    // test_5();
    // test_figure1_GGraph();

    // std::string file_name = argv[1];    
    // std::cout << "--------------------------------" << std::endl;
    // std::cout << "Testing read points with floating point Filtration" << std::endl;
    // test_read_points(file_name);

    std::string file_name = argv[1];   
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Testing read points with discrete GradePoint" << std::endl;
    test_read_points_GradeTable(file_name);
    std::cout << "--------------------------------" << std::endl;
    return 0;
}