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

    boost::timer::cpu_timer overall_timer, io_timer, 
        build_ggraph_timer, 
        create_grades_timer,
        collapse_timer, grades_iteration_timer;
    
    // Initialize/Reset timers (start will clear previous records)
    void initialize_timers() {    
        overall_timer.start();
        overall_timer.stop();
        // io_timer.start();
        // io_timer.stop();
        build_ggraph_timer.start();
        build_ggraph_timer.stop();
        create_grades_timer.start();
        create_grades_timer.stop();
        collapse_timer.start();
        collapse_timer.stop();
        grades_iteration_timer.start();
        grades_iteration_timer.stop();
    }

    void print_timers(double total = double(overall_timer.elapsed().wall) / std::pow(10,9)) {
        std::cout << "Overall timer: " << double(overall_timer.elapsed().wall)/std::pow(10,9) << std::endl;
        
        pretty_print_timer("Collapse to minimal graph:", collapse_timer, total);
        pretty_print_timer("Store grades lexicographically:", create_grades_timer, total);
        pretty_print_timer("Main Loop(visit grades lexicographically):", grades_iteration_timer, total);
    }
}
  
