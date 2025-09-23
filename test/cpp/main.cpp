#include <cassert>
#include <icecream.hpp>
#include <iostream>
#include <tuple>

#include "ContractionTopTree.hpp"
#include "IO.hpp"
#include "MPH.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"

int test_figure1_GGraph() {
    GGraph g(6);
    g.add_vertex(0, GradePoint(6, 2));  // x1
    g.add_vertex(1, GradePoint(6, 2));  // x2
    g.add_vertex(2, GradePoint(6, 2));  // x3
    g.add_vertex(3, GradePoint(1, 3));  // u
    g.add_vertex(4, GradePoint(2, 1));  // v
    g.add_vertex(5, GradePoint(2, 6));  // w

    g.add_edge(0, 1, GradePoint(6, 2));  // d1 = (x1, x2)
    g.add_edge(1, 2, GradePoint(6, 2));  // d2 = (x2,x3)
    g.add_edge(0, 2, GradePoint(6, 2));  // d3 = (x1,x3)
    g.add_edge(3, 4, GradePoint(3, 5));  // e1 = (u,v)
    g.add_edge(3, 4, GradePoint(5, 3));  // e2 = (u,v)
    g.add_edge(4, 0, GradePoint(6, 2));  // h1 = (v, x1)
    g.add_edge(3, 5, GradePoint(2, 6));  // h2 = (u, w)
    g.add_edge(5, 2, GradePoint(6, 6));  // e3 = (w, x3)
    auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0(g);

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

int test_compute_MPH0_from_point_cloud(std::string file_name, std::string filtration_type,
                                       bool save_to_file = false,
                                       std::string file_name_output = "") {
    GGraph ggraph;
    size_t x_size, y_size;
    if (filtration_type == "degree") {
        GradeTable<double, int> grade_table;  // (x, y) is (radius, degree)
        std::tie(ggraph, grade_table) =
            build_degree_filtration_from_point_cloud<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "ball_density") {
        GradeTable<double, double> grade_table;  // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) =
            build_ball_density_filtration_from_point_cloud<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "degree_rivet") {
        GradeTable<int, rivet::ExactValue> grade_table;  // (x, y) is (degree, radius)
        std::tie(ggraph, grade_table) =
            build_degree_filtration_from_point_cloud_rivet<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "ball_density_rivet") {
        GradeTable<rivet::ExactValue, rivet::ExactValue> grade_table;  // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) =
            build_ball_density_filtration_from_point_cloud_rivet<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else {
        std::cout << "Invalid filtration type" << std::endl;
        return 1;
    }

    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0(ggraph);

    // print and write the betti numbers
    if (save_to_file) {
        if (file_name_output == "") file_name_output = "TopTree_out.txt";
        std::cout << "Saving to file: " << file_name_output << std::endl;
        bool x_y_swap = true;
        print_and_write_betti_result(raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1,
                                     x_y_swap, file_name_output);
    }
    return 0;
}

int main(int argc, char** argv) {
    // std::cout << "--------------------------------" << std::endl;
    // std::cout << "Testing figure 1" << std::endl;
    // test_figure1_GGraph();
    // std::cout << std::endl;

    std::string file_name = argv[1];
    std::string filtration_type = argv[2];
    bool save_to_file = false;
    std::string file_name_output = "";
    if (argc > 5 || argc < 3) {
        std::cout << "Invalid number of arguments. Expected: <input_file> "
                     "<filtration_type> <optional_output_file>"
                  << std::endl;
        return 1;
    }
    if (argc == 4) {
        save_to_file = true;
        file_name_output = argv[3];
    }

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Testing " << filtration_type << "-Rips Filtration" << std::endl;
    if (save_to_file) {
        std::cout << "The results will be saved to " << file_name_output << std::endl;
    }
    test_compute_MPH0_from_point_cloud(file_name, filtration_type, save_to_file,
                                       file_name_output);

    return 0;
}