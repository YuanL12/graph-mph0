// Args.hpp
#pragma once

#include <string>

struct ProgramArgs {
    std::string input_file;
    std::string filtration_type;
    bool save_to_file = false;
    std::string output_file = "";
    bool swap_x_y = false;

    bool is_valid() const;
    static void print_usage(const std::string& program_name);
};

ProgramArgs parse_arguments(int argc, char** argv);
