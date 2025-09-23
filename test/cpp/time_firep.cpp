#include <cassert>
#include <icecream.hpp>
#include <iostream>
#include <tuple>

#include "ContractionTopTree.hpp"
#include "IO.hpp"
#include "MPH.hpp"
#include "PointCloud.hpp"
#include "RIVET.hpp"
#include "Timer.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Need 1 arguments: " << std::endl;
        std::cout << "<input_firep_file>" << std::endl;
        return 1;
    }
    std::cout << "--------------------------------" << std::endl;
    std::string file_name = argv[1];
    std::cout << "input file_name: " << file_name << std::endl;

#if MPH0_TIMERS
    mph0::initialize_timers();
    mph0::overall_timer.resume();
    mph0::build_ggraph_timer.resume();
#endif

    // read the file and build the graph
    GGraph ggraph;
    GradeTable<int, int> grade_table;
    std::tie(ggraph, grade_table) = read_filtration_data_from_firep(file_name);
    size_t x_size = grade_table.get_x_size();
    size_t y_size = grade_table.get_y_size();

#if MPH0_TIMERS
    mph0::build_ggraph_timer.stop();
    mph0::overall_timer.stop();  // pause the overall timer
#endif

    // compute active grades size
    size_t active_grades_size = ggraph.get_size_of_active_grades();
    size_t total_grades_size = x_size * y_size;
    std::cout << "Total grades size: (" << x_size << ", " << y_size
              << ") = " << std::scientific << std::setprecision(2)
              << total_grades_size / 1e6 << " M" << std::endl;
    std::cout << "Active grades size: " << std::scientific << std::setprecision(2)
              << active_grades_size / 1e6 << " M";
    // get the percentage of active grades
    std::cout << " (" << static_cast<double>(active_grades_size) / total_grades_size * 100
              << "%)" << std::endl;

#if MPH0_TIMERS
    mph0::overall_timer.resume();
#endif

    // compute betti numbers
    auto [raw_betti_0, raw_betti_1, raw_betti_2, raw_betti_0_1, M] = compute_MPH0(ggraph);

#if MPH0_TIMERS
    mph0::overall_timer.stop();
    mph0::print_timers();
#endif

    std::cout << "Done!" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    return 0;
}