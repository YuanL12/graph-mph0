#include "Args.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

bool ProgramArgs::is_valid() const {
    return !input_file.empty() && !filtration_type.empty() &&
           (filtration_type == "degree" || filtration_type == "ball_density" ||
            filtration_type == "degree_rational" || filtration_type == "ball_density_rational");
}

void ProgramArgs::print_usage(const std::string& program_name) {
    std::cout << "Usage: " << program_name << " <input_file> <filtration_type> [options]\n";
    std::cout << "\nRequired arguments:\n";
    std::cout << "  input_file        Path to input point cloud file\n";
    std::cout << "  filtration_type   Type of filtration: degree, ball_density, degree_rational, "
                 "ball_density_rational\n";
    std::cout << "\nOptional arguments:\n";
    std::cout << "  --output <file>   Save results to specified output file\n";
    std::cout << "  --swap_xy         Enable x-y axis swapping (default: swap disabled)\n";
    std::cout << "  --help            Show this help message\n";
}

ProgramArgs parse_arguments(int argc, char** argv) {
    ProgramArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            ProgramArgs::print_usage(argv[0]);
            std::exit(0);
        }
    }

    if (argc < 3) {
        throw std::invalid_argument("Insufficient arguments");
    }

    args.input_file = argv[1];
    args.filtration_type = argv[2];

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--output" || arg == "-o") {
            if (i + 1 >= argc) {
                throw std::invalid_argument("--output requires a filename");
            }
            args.output_file = argv[++i];
            args.save_to_file = true;
        } else if (arg == "--swap_xy") {
            args.swap_x_y = true;
        } else {
            if (arg[0] != '-') {
                args.output_file = arg;
                args.save_to_file = true;
            } else {
                throw std::invalid_argument("Unknown argument: " + arg);
            }
        }
    }

    if (!args.is_valid()) {
        throw std::invalid_argument("Invalid arguments provided");
    }

    return args;
}
