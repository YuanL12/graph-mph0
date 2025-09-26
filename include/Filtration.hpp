#pragma once

#include <string>
#include <tuple>
#include <variant>

#include "GGraph.hpp"
#include "Grade.hpp"
#include "IO.hpp"
#include "RIVET.hpp"

struct BiFiltration {
    GGraph ggraph;
    GradeTableVariant grade_table;

    // Factory function
    static BiFiltration make(const std::string &filtration_type, const std::string &path);
    static BiFiltration make_from_point_cloud(const std::string &filtration_type,
                                              const std::string &path);
    static BiFiltration make_from_firep(const std::string &path);

    // Helper function to get the vectors in the grade table
    template <typename F>
    inline decltype(auto) with_table(F &&f) {
        return std::visit(std::forward<F>(f), grade_table);
    }
};
