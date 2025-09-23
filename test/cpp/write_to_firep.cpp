#include <icecream.hpp>
#include "IO.hpp"
#include <chrono>
#include "MPH.hpp"
#include "GGraph.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"
#include "RIVET.hpp"

int write_to_firep(
    std::string filtration_type, // ball_density or degree
    std::string file_name,
    std::string output_file_name)
{
    auto points = read_points<double>(file_name);

    GGraph ggraph;
    if (filtration_type == "ball_density")
    {
        GradeTable<double, double> grade_table; // (x, y) is (ball density, radius)
        std::tie(ggraph, grade_table) = point_cloud_to_ball_density_Rips_filtration<double>(points);
    }
    else if (filtration_type == "degree")
    {
        GradeTable<double, int> grade_table; // (x, y) is (radius, degree)
        std::tie(ggraph, grade_table) = point_cloud_to_degree_Rips_filtration<double>(points);
    }
    else
    {
        std::cout << "Invalid filtration type" << std::endl;
        return 1;
    }
    write_filtration_data_to_mpfree_firep(ggraph, output_file_name);
    return 0;
}

int main(int argc, char **argv)
{
    // Parse the command line arguments
    if (argc != 4)
    {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Need 3 arguments: " << std::endl;
        std::cout << "<input_file> <output_file> <filtration_type(ball_density or degree)>" << std::endl;
        return 1;
    }
    std::string file_name = argv[1];
    std::string output_file_name = argv[2];
    std::string filtration_type = argv[3];
    std::cout << "input file_name: " << file_name << std::endl;
    std::cout << "output file_name: " << output_file_name << std::endl;
    std::cout << "filtration type: " << filtration_type << std::endl;
    write_to_firep(filtration_type, file_name, output_file_name);
    std::cout << "Done!" << std::endl;
    return 0;
}