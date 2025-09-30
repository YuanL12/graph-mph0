#pragma once

#include <string>
#include <tuple>
#include <variant>

#include "GGraph.hpp"
#include "Grade.hpp"
#include "IO.hpp"
#include "RIVET.hpp"

class BiFiltration {
   public:
    GGraph ggraph;
    GradeTableVariant grade_table;

    BiFiltration() = default;
    BiFiltration(const GGraph &ggraph, const GradeTableVariant &grade_table)
        : ggraph(ggraph), grade_table(grade_table) {}

    // Construct from a file
    BiFiltration(const std::string &filtration_type, const std::string &path);

    // Construct from a point cloud(2D array)
    BiFiltration(const std::vector<std::vector<PointCloudType>> &points,
                 const std::string &filtration_type);

    // Construct Function-Rips Bifiltration from a point cloud(2D array) and a function value)
    BiFiltration(const std::vector<std::vector<PointCloudType>> &points,
                 const std::vector<double> &function_values,
                 std::optional<double> max_function_value = std::nullopt,
                 std::optional<PointCloudType> max_distance = std::nullopt);

    CoordsVariant get_x_coords() const;
    CoordsVariant get_y_coords() const;

   private:
    BiFiltration make_from_point_cloud(const std::vector<std::vector<PointCloudType>> &points,
                                       const std::string &filtration_type);
    BiFiltration make_from_firep(const std::string &path);
    BiFiltration make_from_function_Rips(const std::vector<std::vector<PointCloudType>> &points,
                                         const std::vector<double> &function_values,
                                         std::optional<double> max_function_value,
                                         std::optional<PointCloudType> max_distance);

    // Helper function to get the vectors in the grade table
    template <typename F>
    inline decltype(auto) with_table(F &&f) {
        return std::visit(std::forward<F>(f), grade_table);
    }
};
