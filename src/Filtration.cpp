
#include "Filtration.hpp"

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

BiFiltration BiFiltration::make_from_function_Rips(
    const std::vector<std::vector<PointCloudType>> &points,
    const std::vector<double> &function_values, std::optional<double> max_function_value,
    std::optional<PointCloudType> max_distance) {
    auto [g, gt] = point_cloud_to_function_Rips_filtration<PointCloudType>(
        points, function_values, max_function_value, max_distance);
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

BiFiltration::BiFiltration(const std::vector<std::vector<PointCloudType>> &points,
                           const std::vector<double> &function_values,
                           std::optional<double> max_function_value,
                           std::optional<PointCloudType> max_distance) {
    *this = make_from_function_Rips(points, function_values, max_function_value, max_distance);
}

CoordsVariant BiFiltration::get_x_coords() const {
    return std::visit([&](auto &&tbl) -> CoordsVariant { return tbl.get_x_coords(); }, grade_table);
}

CoordsVariant BiFiltration::get_y_coords() const {
    return std::visit([&](auto &&tbl) -> CoordsVariant { return tbl.get_y_coords(); }, grade_table);
}