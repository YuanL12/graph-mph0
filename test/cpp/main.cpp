#include <cassert>
#include <icecream.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "Args.hpp"
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
                                       bool save_to_file = false, std::string file_name_output = "",
                                       bool x_y_swap = false) {
    GGraph ggraph;
    size_t x_size, y_size;
    if (filtration_type == "degree") {
        GradeTable<double, int> grade_table;  // (x, y) is (radius, degree)
        std::tie(ggraph, grade_table) = build_degree_filtration_from_point_cloud<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "ball_density") {
        GradeTable<double, double> grade_table;  // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) =
            build_ball_density_filtration_from_point_cloud<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "degree_rational") {
        GradeTable<int, rivet::ExactValue> grade_table;  // (x, y) is (degree, radius)
        std::tie(ggraph, grade_table) =
            build_degree_filtration_from_point_cloud_rational<double>(file_name);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    } else if (filtration_type == "ball_density_rational") {
        GradeTable<rivet::ExactValue, rivet::ExactValue>
            grade_table;  // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) =
            build_ball_density_filtration_from_point_cloud_rational<double>(file_name);
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
        print_and_write_betti_result(raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, x_y_swap,
                                     file_name_output);
    }
    return 0;
}

int main(int argc, char** argv) {
    // std::cout << "--------------------------------" << std::endl;
    // std::cout << "Testing figure 1" << std::endl;
    // test_figure1_GGraph();
    // std::cout << std::endl;

    try {
        // Parse command line arguments
        ProgramArgs args = parse_arguments(argc, argv);

        std::cout << "--------------------------------" << std::endl;
        std::cout << "Testing " << args.filtration_type << "-Rips Filtration" << std::endl;
        std::cout << "Input file: " << args.input_file << std::endl;
        std::cout << "Swap x, y axis: " << (args.swap_x_y ? "enabled" : "disabled") << std::endl;

        if (args.save_to_file) {
            std::cout << "The results will be saved to " << args.output_file << std::endl;
        }

        test_compute_MPH0_from_point_cloud(args.input_file, args.filtration_type, args.save_to_file,
                                           args.output_file, args.swap_x_y);

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << std::endl;
        ProgramArgs::print_usage(argv[0]);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}