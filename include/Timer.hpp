/* 
    Inspired from mpfree by Michael Kerber
    Copyright 2021 TU Graz
*/

#pragma once
#include <iostream>
#include <boost/timer/timer.hpp>
#include <cmath>
#include <iomanip>  // for std::setw, std::setprecision
#include <cmath>    // for std::pow

void pretty_print_timer(const std::string& label, const boost::timer::cpu_timer& timer, double total_seconds) {
    double seconds = double(timer.elapsed().wall) / 1e9;
    double percent = (seconds / total_seconds) * 100;

    std::cout << std::left << std::setw(50) << label     // left-aligned label, 30 characters wide
              << std::right << std::fixed << std::setw(8) << std::setprecision(3) << seconds << " s"
              << "   ( " << std::setw(6) << std::setprecision(2) << percent << " % )" << std::endl;
}

namespace mph0 {

    boost::timer::cpu_timer overall_timer, load_input_timer, 
        build_ggraph_timer,
        collapse_edge_timer, collapse_vertex_timer, 
        update_graph_from_collapse_edge_timer, update_graph_from_collapse_vertex_timer,
        create_grades_timer,
        grades_iteration_timer;
    
    // Initialize/Reset timers (start will clear previous records)
    void initialize_timers() {    
        overall_timer.start();
        overall_timer.stop();

        load_input_timer.start();
        load_input_timer.stop();
 
        build_ggraph_timer.start();
        build_ggraph_timer.stop();
        
        collapse_edge_timer.start();
        collapse_edge_timer.stop();
        
        collapse_vertex_timer.start();
        collapse_vertex_timer.stop();
        
        update_graph_from_collapse_edge_timer.start();
        update_graph_from_collapse_edge_timer.stop();
        
        update_graph_from_collapse_vertex_timer.start();
        update_graph_from_collapse_vertex_timer.stop();

        create_grades_timer.start();
        create_grades_timer.stop();
        
        grades_iteration_timer.start();
        grades_iteration_timer.stop();
    }

    void print_timers(double total = double(overall_timer.elapsed().wall) / std::pow(10,9)) {
        pretty_print_timer("Load input:", load_input_timer, total);
        pretty_print_timer("Build ggraph:", build_ggraph_timer, total);
        pretty_print_timer("Collapse edge:", collapse_edge_timer, total);
        pretty_print_timer("Update graph inside collapse edge:", update_graph_from_collapse_edge_timer, total);
        pretty_print_timer("Collapse vertex:", collapse_vertex_timer, total);
        pretty_print_timer("Update graph inside collapse vertex:", update_graph_from_collapse_vertex_timer, total);
        pretty_print_timer("Collect grades lexicographically:", create_grades_timer, total);
        pretty_print_timer("Main Loop(visit grades lexicographically):", grades_iteration_timer, total);
        
        double load_input_time = double(load_input_timer.elapsed().wall)/std::pow(10,9);
        std::cout << "Total time: " << total << " s" << std::endl;
        std::cout << "Total time minus load input: " << total - load_input_time << " s" << std::endl;
    }
}
  
