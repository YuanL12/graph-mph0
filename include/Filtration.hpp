#pragma once

#include <string>
#include <tuple>
#include <variant>

#include "GGraph.hpp"
#include "Grade.hpp"
#include "IO.hpp"
#include "RIVET.hpp"

// template <typename PT>
// std::tuple<GGraph, GradeTable<PT, int>> build_degree_filtration_from_point_cloud(
//     const std::string &filename) {
// #if MPH0_TIMERS
//     mph0::load_input_timer.resume();
// #endif

//     // read the points
//     auto points = read_points<PT>(filename);

// #if MPH0_TIMERS
//     mph0::load_input_timer.stop();
// #endif

//     // build the degree filtration
//     return point_cloud_to_degree_Rips_filtration<PT>(points);
// }

// template <typename PT>
// std::tuple<GGraph, GradeTable<double, PT>> build_ball_density_filtration_from_point_cloud(
//     const std::string &filename) {
// #if MPH0_TIMERS
//     mph0::load_input_timer.resume();
// #endif

//     // read the points
//     auto points = read_points<PT>(filename);

// #if MPH0_TIMERS
//     mph0::load_input_timer.stop();
// #endif

//     // build the ball density filtration
//     return point_cloud_to_ball_density_Rips_filtration<PT>(points);
// }

// template <typename PT>
// std::tuple<GGraph, GradeTable<int, rivet::ExactValue>>
// build_degree_filtration_from_point_cloud_rational(const std::string &filename) {
//     // read the points
//     auto points = read_points<PT>(filename);
//     return point_cloud_to_degree_Rips_filtration_rational<PT>(points);
// }

// template <typename PT>
// std::tuple<GGraph, GradeTable<rivet::ExactValue, rivet::ExactValue>>
// build_ball_density_filtration_from_point_cloud_rational(const std::string &filename) {
//     // read the points
//     auto points = read_points<PT>(filename);
//     return point_cloud_to_ball_density_Rips_filtration_rational<PT>(points);
// }

// -----------------------------------------------------------------------------
// Result types and runtime facade for variant-based access (Python-friendly)

using GradeTableVariant =
    std::variant<GradeTable<double, int>,                           // (radius, degree)
                 GradeTable<double, double>,                        // (ball density, radius)
                 GradeTable<int, rivet::ExactValue>,                // (degree, radius)
                 GradeTable<rivet::ExactValue, rivet::ExactValue>,  // (ball density, radius)
                 GradeTable<int, int>>;                             // firep (x_int, y_int)

struct BiFiltration {
    GGraph ggraph;
    GradeTableVariant grade_table;

    // Factory function
    static BiFiltration make(const std::string &filtration_type, const std::string &path);
    static BiFiltration make_from_point_cloud(const std::string &filtration_type,
                                              const std::string &path);
    static BiFiltration make_from_firep(const std::string &path);
};

// class FiltrationRunner {
//    public:
//     static FiltrationResultVariant build(const std::string &filtration_type,
//                                          const std::string &path);

//    private:
//     static FiltrationResultVariant build_from_point_cloud(const std::string &filtration_type,
//                                                           const std::string &path);
//     static FiltrationResultVariant build_from_firep(const std::string &path);
// };