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
    std::string output_file_name,
    bool write_stages)
{
    std::vector<std::vector<double>> points;
    if (filtration_type != "firep") points = read_points<double>(file_name);

    GGraph ggraph;
    if (filtration_type == "firep")
    {
        GradeTable<int, int> grade_table;
        std::tie(ggraph, grade_table) = read_filtration_data_from_firep(file_name);
    }
    else if (filtration_type == "ball_density")
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
    if (!write_stages) {
        write_filtration_data_to_mpfree_firep(ggraph, output_file_name);
        return 0;
    }

    write_filtration_data_to_mpfree_firep(ggraph, output_file_name + "_raw.firep");

    GGraph alg1 = ggraph;
    alg1.initialize_active_vertices_flags();
    local_collapse_edges_Grade_Version(alg1);
    write_filtration_data_to_mpfree_firep(alg1, output_file_name + "_alg1.firep");

    GGraph vertex_minimal = ggraph;
    vertex_minimal.initialize_active_vertices_flags();
    collapse_to_vertex_minimal_Grade_Version(vertex_minimal);
    write_filtration_data_to_mpfree_firep(vertex_minimal, output_file_name + "_vertex_minimal.firep");
    return 0;
}

int main(int argc, char **argv)
{
    // Parse the command line arguments
    if (argc != 4 && argc != 5)
    {
        std::cout << "--------------------------------" << std::endl;
        std::cout << "Need 3 arguments: " << std::endl;
        std::cout << "<input_file> <output_file_or_prefix> <filtration_type(ball_density, degree, or firep)> [--stages]" << std::endl;
        return 1;
    }
    std::string file_name = argv[1];
    std::string output_file_name = argv[2];
    std::string filtration_type = argv[3];
    std::cout << "input file_name: " << file_name << std::endl;
    std::cout << "output file_name: " << output_file_name << std::endl;
    std::cout << "filtration type: " << filtration_type << std::endl;
    bool write_stages = argc == 5 && std::string(argv[4]) == "--stages";
    if (argc == 5 && !write_stages) {
        std::cerr << "Unknown argument: " << argv[4] << std::endl;
        return 1;
    }
    int status = write_to_firep(filtration_type, file_name, output_file_name, write_stages);
    if (status == 0) std::cout << "Done!" << std::endl;
    return status;
}