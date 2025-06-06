#include "MPH.hpp"
#include <icecream.hpp>
#include "IO.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"
#include "ContractionTopTree.hpp"
#include <iostream>
#include <tuple>
#include <cassert>

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
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_TopTree(g);
    
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


int test_degree_Rips_filtration_LCT(std::string file_name) {
#if MPH0_TIMERS
mph0::initialize_timers();
mph0::overall_timer.start();
#endif
    // read the file
    auto points = read_points<double>(file_name);

    // build the graph
#if MPH0_TIMERS
mph0::build_ggraph_timer.start();
#endif
    auto [ggraph, grade_table] = point_cloud_to_degree_Rips_filtration<double>(points);
#if MPH0_TIMERS
mph0::build_ggraph_timer.stop();
#endif

    // time the computation
    std::cout << "Computing MPH0... by LCT" << std::endl;
    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0_DTree_Grade_Version(ggraph);

#if MPH0_TIMERS
mph0::overall_timer.stop();
mph0::print_timers();
#endif
    return 0;
}




int test_degree_Rips_filtration_TopTree(std::string file_name) {
#if MPH0_TIMERS
mph0::initialize_timers();
mph0::overall_timer.start();
#endif
    // read the file
    auto points = read_points<double>(file_name);

    // build the graph
#if MPH0_TIMERS
mph0::build_ggraph_timer.start();
#endif
    auto [ggraph, grade_table] = point_cloud_to_degree_Rips_filtration<double>(points);
#if MPH0_TIMERS
mph0::build_ggraph_timer.stop();
#endif

    // time the computation
    std::cout << "Computing MPH0... by TopTree" << std::endl;
    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0_TopTree(ggraph);

#if MPH0_TIMERS
mph0::overall_timer.stop();
mph0::print_timers();
#endif
    return 0;
}



int main(int argc, char** argv) {
    // test_figure1_GGraph();
    std::string file_name = argv[1];

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Testing degree Rips Filtration by LCT" << std::endl;
    test_degree_Rips_filtration_LCT(file_name);
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Testing degree Rips Filtration by TopTree" << std::endl;
    test_degree_Rips_filtration_TopTree(file_name);

    return 0;
} 