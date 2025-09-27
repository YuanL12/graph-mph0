
#include "Filtration.hpp"

// These wrappers simply forward to the existing template functions in IO.hpp.

// std::tuple<GGraph, GradeTable<double, int>> build_degree_filtration_from_point_cloud(
//     const std::string &filename) {
//     return build_degree_filtration_from_point_cloud<double>(filename);
// }

// std::tuple<GGraph, GradeTable<double, double>> build_ball_density_filtration_from_point_cloud(
//     const std::string &filename) {
//     return build_ball_density_filtration_from_point_cloud<double>(filename);
// }

// std::tuple<GGraph, GradeTable<int, rivet::ExactValue>>
// build_degree_filtration_from_point_cloud_rational(const std::string &filename) {
//     return build_degree_filtration_from_point_cloud_rational<double>(filename);
// }

// std::tuple<GGraph, GradeTable<rivet::ExactValue, rivet::ExactValue>>
// build_ball_density_filtration_from_point_cloud_rational(const std::string &filename) {
//     return build_ball_density_filtration_from_point_cloud_rational<double>(filename);
// }

// The implementation of read_filtration_data_from_firep already exists in IO.hpp/IO.cpp context.
// We just rely on the declaration from IO.hpp and link against it.

BiFiltration BiFiltration::make_from_point_cloud(
    const std::vector<std::vector<PointCloudType>> &points, const std::string &filtration_type) {
    if (filtration_type == "degree") {
        auto [g, gt] = point_cloud_to_degree_Rips_filtration<PointCloudType>(points);
        return BiFiltration{std::move(g), std::move(gt)};
    } else if (filtration_type == "ball_density") {
        auto [g, gt] = point_cloud_to_ball_density_Rips_filtration<PointCloudType>(points);
        return BiFiltration{std::move(g), std::move(gt)};
    } else if (filtration_type == "degree_rational") {
        auto [g, gt] = point_cloud_to_degree_Rips_filtration_rational<PointCloudType>(points);
        return BiFiltration{std::move(g), std::move(gt)};
    } else if (filtration_type == "ball_density_rational") {
        auto [g, gt] = point_cloud_to_ball_density_Rips_filtration_rational<PointCloudType>(points);
        return BiFiltration{std::move(g), std::move(gt)};
    } else {
        throw std::invalid_argument("Unknown BiFiltration type to load from a point cloud");
    }
}

BiFiltration BiFiltration::make_from_firep(const std::string &path) {
    auto [g, gt] = read_filtration_data_from_firep(path);
    return BiFiltration{std::move(g), std::move(gt)};
}

BiFiltration::BiFiltration(const std::string &filtration_type, const std::string &path) {
    if (filtration_type == "firep") {
        *this = make_from_firep(path);
    } else {
// record the time to read the points
#if MPH0_TIMERS
        mph0::load_input_timer.resume();
#endif
        // read the points
        auto points = read_points<PointCloudType>(path);
#if MPH0_TIMERS
        mph0::load_input_timer.stop();
#endif
        *this = make_from_point_cloud(points, filtration_type);
    }
}

BiFiltration::BiFiltration(const std::vector<std::vector<PointCloudType>> &points,
                           const std::string &filtration_type) {
    *this = make_from_point_cloud(points, filtration_type);
}

CoordsVariant BiFiltration::get_x_coords() const {
    return std::visit([&](auto &&tbl) -> CoordsVariant { return tbl.get_x_coords(); }, grade_table);
}

CoordsVariant BiFiltration::get_y_coords() const {
    return std::visit([&](auto &&tbl) -> CoordsVariant { return tbl.get_y_coords(); }, grade_table);
}

// FiltrationResultVariant FiltrationRunner::build_from_point_cloud(const std::string
// &filtration_type,
//                                                                  const std::string &path) {
//     // record the time to read the points
// #if MPH0_TIMERS
//     mph0::load_input_timer.resume();
// #endif
//     // read the points
//     auto points = read_points<PointCloudType>(path);
// #if MPH0_TIMERS
//     mph0::load_input_timer.stop();
// #endif

//     if (filtration_type == "degree") {
//         auto [g, gt] = point_cloud_to_degree_Rips_filtration<PointCloudType>(points);
//         return FiltrationResult<double, int>{std::move(g), std::move(gt)};
//     } else if (filtration_type == "ball_density") {
//         auto [g, gt] = point_cloud_to_ball_density_Rips_filtration<PointCloudType>(points);
//         return FiltrationResult<double, double>{std::move(g), std::move(gt)};
//     } else if (filtration_type == "degree_rational") {
//         auto [g, gt] = point_cloud_to_degree_Rips_filtration_rational<PointCloudType>(points);
//         return FiltrationResult<int, rivet::ExactValue>{std::move(g), std::move(gt)};
//     } else if (filtration_type == "ball_density_rational") {
//         auto [g, gt] =
//         point_cloud_to_ball_density_Rips_filtration_rational<PointCloudType>(points); return
//         FiltrationResult<rivet::ExactValue, rivet::ExactValue>{std::move(g), std::move(gt)};
//     } else {
//         throw std::invalid_argument("Unknown FiltrationKind to load from a point cloud");
//     }
// }

// FiltrationResultVariant FiltrationRunner::build_from_firep(const std::string &path) {
//     auto [g, gt] = read_filtration_data_from_firep(path);
//     return FiltrationResult<int, int>{std::move(g), std::move(gt)};
// }

// FiltrationResultVariant FiltrationRunner::build(const std::string &filtration_type,
//                                                 const std::string &path) {
//     if (filtration_type == "firep") {
//         return build_from_firep(path);
//     } else {
//         return build_from_point_cloud(filtration_type, path);
//     }
// }