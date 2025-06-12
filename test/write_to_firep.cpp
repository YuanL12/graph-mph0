#include <icecream.hpp>
#include "IO.hpp"
#include <chrono>
#include "MPH.hpp"
#include "GGraph.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"

int test_write_ball_density_rips_filtration_to_firep(std::string file_name, std::string output_file_name) {
    auto points = read_points<double>(file_name);
    size_t num_points = points.size();

    auto time_start = std::chrono::high_resolution_clock::now();
    auto [ggraph, grade_table] = point_cloud_to_ball_density_Rips_filtration<double>(points);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_duration = time_end - time_start;
    std::cout << "Time taken to build the graph and grade table: " << time_duration.count() << " seconds" << std::endl;

    // write the filtration data to scc2020 format
    time_start = std::chrono::high_resolution_clock::now();
    write_filtration_data_to_scc2020(ggraph, output_file_name);
    time_end = std::chrono::high_resolution_clock::now();
    time_duration = time_end - time_start;
    std::cout << "Time taken to write the filtration data to scc2020 format: " << time_duration.count() << " seconds" << std::endl;
    
    return 0;
}




int test_write_deg_rips_filtration_to_firep(std::string file_name, std::string output_file_name) {
    auto points = read_points<double>(file_name);
    size_t num_points = points.size();

    auto time_start = std::chrono::high_resolution_clock::now();
    auto [ggraph, grade_table] = point_cloud_to_degree_Rips_filtration<double>(points);
    auto time_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_duration = time_end - time_start;
    std::cout << "Time taken to build the graph and grade table: " << time_duration.count() << " seconds" << std::endl;

    // write the filtration data to scc2020 format
    time_start = std::chrono::high_resolution_clock::now();
    write_filtration_data_to_rivet_firep(ggraph, output_file_name);
    time_end = std::chrono::high_resolution_clock::now();
    time_duration = time_end - time_start;
    std::cout << "Time taken to write the filtration data to scc2020 format: " << time_duration.count() << " seconds" << std::endl;
    
    return 0;
}


int main(int argc, char** argv) {
    // Parse the command line arguments
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return 1;
    }
    std::string file_name = argv[1];   
    std::string output_file_name = argv[2];
    std::cout << "file_name: " << file_name << std::endl;
    std::cout << "output_file_name: " << output_file_name << std::endl;

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Write graded graph to scc2020 format" << std::endl;
    test_write_deg_rips_filtration_to_firep(file_name, output_file_name);

    return 0;
}