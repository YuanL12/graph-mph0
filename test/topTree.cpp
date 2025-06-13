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
    g.add_vertex(0, GradePoint(6,2)); // x1
    g.add_vertex(1, GradePoint(6,2)); // x2
    g.add_vertex(2, GradePoint(6,2)); // x3
    g.add_vertex(3, GradePoint(1,3)); // u
    g.add_vertex(4, GradePoint(2,1)); // v 
    g.add_vertex(5, GradePoint(2,6)); // w

    g.add_edge(0, 1, GradePoint(6,2)); // d1 = (x1, x2)
    g.add_edge(1, 2, GradePoint(6,2)); // d2 = (x2,x3)
    g.add_edge(0, 2, GradePoint(6,2)); // d3 = (x1,x3)
    g.add_edge(3, 4, GradePoint(3,5)); // e1 = (u,v)
    g.add_edge(3, 4, GradePoint(5,3)); // e2 = (u,v)
    g.add_edge(4, 0, GradePoint(6,2)); // h1 = (v, x1)
    g.add_edge(3, 5, GradePoint(2,6)); // h2 = (u, w)
    g.add_edge(5, 2, GradePoint(6,6)); // e3 = (w, x3)
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


int test_TopTree(std::string file_name, bool save_to_file = false, std::string file_name_output = "") {
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
    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0_TopTree(ggraph);

#if MPH0_TIMERS
mph0::overall_timer.stop();
mph0::print_timers();
#endif

    // print and write the betti numbers
    if (save_to_file) {
        if (file_name_output == "") file_name_output = "TopTree_out.txt";
        std::cout << "Saving to file: " << file_name_output << std::endl;
        bool x_y_swap = false;
        print_and_write_betti_result(raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, 
                                    x_y_swap, file_name_output);
    }
    return 0;
}



int main(int argc, char** argv) {
    std::cout << "--------------------------------" << std::endl;

    // std::cout << "Testing figure 1" << std::endl;
    // std::cout << "--------------------------------" << std::endl;
    // test_figure1_GGraph();

    std::string file_name = argv[1];
    std::string output_file_name = "";
    bool save_to_file = false;
    if (argc > 2) {
        output_file_name = argv[2];
        save_to_file = true;
    }
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Testing degree Rips Filtration" << std::endl;
    std::cout << "Computing MPH0 by TopTree" << std::endl;
    test_TopTree(file_name, save_to_file, output_file_name);

    return 0;
} 