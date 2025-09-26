#pragma once
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "Compare.hpp"
#include "Hash.hpp"
#include "RIVET.hpp"
#include "Types.hpp"

// GradePoint: a point in the grid of grade Table with a partial order
struct GradePoint {
    int x = -1;  // rank in x coordinate of GradeTable
    int y = -1;  // rank in y coordinate of GradeTable

    // Default constructor
    GradePoint() : x(-1), y(-1) {}

    GradePoint(int x_, int y_) : x(x_), y(y_) {}
    int get_x() const { return x; }
    int get_y() const { return y; }

    // less than
    bool less_than(const GradePoint &other) const {
        return (*this != other) && (x <= other.x) && (y <= other.y);
    }

    static bool lexicographical_less_than(const GradePoint &a, const GradePoint &b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }

    // greater than
    bool greater_than(const GradePoint &other) const { return other.less_than(*this); }

    // equal to
    bool operator==(const GradePoint &other) const { return x == other.x && y == other.y; }

    // not equal to
    bool operator!=(const GradePoint &other) const { return !(*this == other); }

    // Less than operator
    bool operator<(const GradePoint &other) const { return less_than(other); }

    // Greater than operator
    bool operator>(const GradePoint &other) const { return greater_than(other); }

    // print operator
    friend std::ostream &operator<<(std::ostream &os, const GradePoint &gp) {
        os << "(" << gp.x << ", " << gp.y << ")";
        return os;
    }
};

// Hash function for GradePoint
template <>
struct std::hash<GradePoint> {
    size_t operator()(const GradePoint &gp) const {
        return std::hash<int>()(gp.x) ^ std::hash<int>()(gp.y);
    }
};

// LexicographicalOrderGradePoint for sorting and unique indexing
struct LexicographicalOrderGradePoint {
    bool operator()(const GradePoint &a, const GradePoint &b) const {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

/*
    GradeTable: a table of grades, x and y coordinates are sorted in ascending order.
    ------------------------------------------------------------
    Args:
        VTX: type of x coordinates
        VTY: type of y coordinates
*/
template <typename VTX, typename VTY>
class GradeTable {
   private:
    // Sets of x and y coordinates for discretization
    std::vector<VTX> x_coords;  // sorted x coordinates
    std::vector<VTY> y_coords;  // sorted y coordinates

   public:
    // Default constructor
    GradeTable() = default;

    // Constructor by x_coords and y_coords
    GradeTable(const std::vector<VTX> &x_coords, const std::vector<VTY> &y_coords)
        : x_coords(x_coords), y_coords(y_coords) {}

    // Get index of x coordinate (expensive operation O(log(n)))
    VTX get_x_value(int x_rank) const {
        if (x_rank >= x_coords.size()) {
            throw std::runtime_error("Error: x rank " + std::to_string(x_rank) +
                                     " coordinate not found in GradeTable");
        }
        return x_coords[x_rank];
    }

    // Get index of y coordinate (expensive operation O(log(n)))
    VTY get_y_value(int y_rank) const {
        if (y_rank >= y_coords.size()) {
            throw std::runtime_error("Error: y rank " + std::to_string(y_rank) +
                                     " coordinate not found in GradeTable");
        }
        return y_coords[y_rank];
    }

    // Get sizes
    size_t get_x_size() const { return x_coords.size(); }
    size_t get_y_size() const { return y_coords.size(); }
    std::vector<VTX> get_x_coords() const { return x_coords; }
    std::vector<VTY> get_y_coords() const { return y_coords; }

    // print
    void print(bool xy_swap = false) const {
        if (!xy_swap) {
            std::cout << "GradeTable(" << get_x_size() << ", " << get_y_size() << ")" << std::endl;
            std::cout << "x_coords: ";
            for (const auto &x : x_coords) {
                std::cout << x << " ";
            }
            std::cout << std::endl;
            std::cout << "y_coords: ";
            for (const auto &y : y_coords) {
                std::cout << y << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "GradeTable(" << get_y_size() << ", " << get_x_size() << ")" << std::endl;
            std::cout << "x_coords: ";
            for (const auto &y : y_coords) {
                std::cout << y << " ";
            }
            std::cout << std::endl;
            std::cout << "y_coords: ";
            for (const auto &x : x_coords) {
                std::cout << x << " ";
            }
            std::cout << std::endl;
        }
    }
};

// GradeTableVariant: a variant type for different GradeTable types
using GradeTableVariant =
    std::variant<GradeTable<double, int>,                           // (radius, degree)
                 GradeTable<double, double>,                        // (ball density, radius)
                 GradeTable<int, rivet::ExactValue>,                // (degree, radius)
                 GradeTable<rivet::ExactValue, rivet::ExactValue>,  // (ball density, radius)
                 GradeTable<int, int>>;                             // firep (x_int, y_int)

// Variant that holds references to vectors
using CoordsVariant =
    std::variant<std::vector<double>, std::vector<int>, std::vector<rivet::ExactValue>>;