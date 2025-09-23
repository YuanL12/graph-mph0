/**
 * Test the read_filtration_data_from_firep function
 */

#include <cassert>
#include <iostream>
#include <tuple>

#include "IO.hpp"

int main(int argc, char** argv) {
    std::string file_name = argv[1];
    auto [ggraph, grade_table] = read_filtration_data_from_firep(file_name);
    std::cout << "ggraph: " << ggraph.get_nedges() << " " << ggraph.get_nvertices()
              << std::endl;
    std::cout << "grade_table: " << grade_table.get_x_size() << " "
              << grade_table.get_y_size() << std::endl;
    return 0;
}
