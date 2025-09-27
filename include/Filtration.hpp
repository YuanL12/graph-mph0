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
    BiFiltration(const std::string &filtration_type, const std::string &path);
    BiFiltration(const std::vector<std::vector<PointCloudType>> &points,
                 const std::string &filtration_type);
    CoordsVariant get_x_coords() const;
    CoordsVariant get_y_coords() const;

   private:
    BiFiltration make_from_point_cloud(const std::vector<std::vector<PointCloudType>> &points,
                                       const std::string &filtration_type);
    BiFiltration make_from_firep(const std::string &path);

    // Helper function to get the vectors in the grade table
    template <typename F>
    inline decltype(auto) with_table(F &&f) {
        return std::visit(std::forward<F>(f), grade_table);
    }
};
