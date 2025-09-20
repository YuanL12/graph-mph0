#include "MPH.hpp"
#include <icecream.hpp>
#include "IO.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"
#include "ContractionTopTree.hpp"
#include <iostream>
#include <tuple>
#include <cassert>
#include "RIVET.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Need 2 arguments: " << std::endl;
        std::cout << "<input_file> <filtration_type(ball_density or degree)>" << std::endl;
        return 1;
    }
    std::cout << "--------------------------------" << std::endl;
    std::string file_name = argv[1];
    std::string filtration_type = argv[2];
    std::cout << "input file_name: " << file_name << std::endl;
    std::cout << "filtration type: " << filtration_type << std::endl;

#if MPH0_TIMERS
    mph0::initialize_timers();
    mph0::overall_timer.resume();
    mph0::load_input_timer.resume();
#endif

    // read the file
    auto points = read_points<double>(file_name);
#if MPH0_TIMERS
    mph0::load_input_timer.stop();
#endif

    // build the graph
#if MPH0_TIMERS
    mph0::build_ggraph_timer.resume();
#endif
    GGraph ggraph;
    size_t x_size, y_size;
    if (filtration_type == "degree")
    {
        GradeTable<double, int> grade_table; // (x, y) is (radius, degree)
        std::tie(ggraph, grade_table) = point_cloud_to_degree_Rips_filtration<double>(points);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    }
    else if (filtration_type == "ball_density")
    {
        GradeTable<double, double> grade_table; // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) = point_cloud_to_ball_density_Rips_filtration<double>(points);
        x_size = grade_table.get_x_size();
        y_size = grade_table.get_y_size();
    }
    else
    {
        std::cout << "Invalid filtration type" << std::endl;
        return 1;
    }
#if MPH0_TIMERS
    mph0::build_ggraph_timer.stop();
#endif

#if MPH0_TIMERS
    mph0::overall_timer.stop(); // pause the overall timer
#endif

    // compute active grades size
    size_t active_grades_size = ggraph.get_size_of_active_grades();
    size_t total_grades_size = x_size * y_size;
    std::cout << "Total grades size: (" << x_size << ", " << y_size << ") = " << std::scientific << std::setprecision(2) << total_grades_size / 1e6 << " M" << std::endl;
    std::cout << "Active grades size: " << std::scientific << std::setprecision(2) << active_grades_size / 1e6 << " M";
    // get the percentage of active grades
    std::cout << " (" << static_cast<double>(active_grades_size) / total_grades_size * 100 << "%)" << std::endl;

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