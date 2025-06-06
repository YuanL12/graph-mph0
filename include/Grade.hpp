#pragma once
#include <iostream>
#include <utility> 
#include <set>
#include <unordered_map>
#include <tuple>
#include <optional>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include "Hash.hpp"
#include <map>
#include "Compare.hpp"
#include "Types.hpp"

// GradePoint: a point in the grid of grade Table with a partial order 
struct GradePoint {
    int x = -1; // rank in x coordinate of GradeTable
    int y = -1; // rank in y coordinate of GradeTable

    // Default constructor
    GradePoint() : x(-1), y(-1) {}
    
    GradePoint(int x_, int y_) : x(x_), y(y_) {}
    int get_x() const { return x; }
    int get_y() const { return y; }

    // less than
    bool less_than(const GradePoint& other) const {
        if (x < other.x){
            if (y <= other.y){
                return true;
            } 
        } else if (x == other.x){
            if (y < other.y){
                return true;
            }
        }
        return false;
    }

    // greater than
    bool greater_than(const GradePoint& other) const {
        if (x > other.x){
            if (y >= other.y){
                return true;
            }
        } else if (x == other.x){
            if (y > other.y){
                return true;
            }
        }
        return false;
    }

    // equal to
    bool operator==(const GradePoint& other) const {
        return x == other.x && y == other.y;
    }

    // not equal to
    bool operator!=(const GradePoint& other) const {
        return !(*this == other);
    }
    
    // Less than operator 
    bool operator<(const GradePoint& other) const {
        return less_than(other);
    }

    // Greater than operator 
    bool operator>(const GradePoint& other) const {
        return greater_than(other);
    }

    // print operator
    friend std::ostream& operator<<(std::ostream& os, const GradePoint& gp) {
        os << "(" << gp.x << ", " << gp.y << ")";
        return os;
    }
};


// Hash function for GradePoint
template<>
struct std::hash<GradePoint> {
    size_t operator()(const GradePoint& gp) const {
        return std::hash<int>()(gp.x) ^ std::hash<int>()(gp.y);
    }
};

// LexicographicalOrderGradePoint for sorting and unique indexing
struct LexicographicalOrderGradePoint {
    bool operator()(const GradePoint& a, const GradePoint& b) const {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

template <typename VTX, typename VTY>
class GradeTable {
private:
    // Sets of x and y coordinates for discretization
    std::vector<VTX> x_coords; // sorted x coordinates
    std::vector<VTY> y_coords; // sorted y coordinates

public:
    // Default constructor
    GradeTable() = default;

    // Constructor by x_coords and y_coords 
    GradeTable(const std::vector<VTX>& x_coords, 
               const std::vector<VTY>& y_coords) 
        : x_coords(x_coords), y_coords(y_coords) {}

    // Get index of x coordinate (expensive operation O(log(n)))
    VTX get_x_value(int x_rank) const {
        if (x_rank >= x_coords.size()) {
            throw std::runtime_error("Error: x rank " + std::to_string(x_rank) + " coordinate not found in GradeTable");
        }
        return x_coords[x_rank];
    }

    // Get index of y coordinate (expensive operation O(log(n)))
    VTY get_y_value(int y_rank) const {
        if (y_rank >= y_coords.size()) {
            throw std::runtime_error("Error: y rank " + std::to_string(y_rank) + " coordinate not found in GradeTable");
        }
        return y_coords[y_rank];
    }

    // Get sizes
    size_t get_x_size() const { return x_coords.size(); }
    size_t get_y_size() const { return y_coords.size(); }

    // print
    void print(bool xy_swap = false) const {
        if (!xy_swap){
            std::cout << "GradeTable(" << get_x_size() << ", " << get_y_size() << ")" << std::endl;
            std::cout << "x_coords: ";
            for (const auto& x : x_coords) {
                std::cout << x << " ";
            }
            std::cout << std::endl;
            std::cout << "y_coords: ";
            for (const auto& y : y_coords) {
                std::cout << y << " ";
            }
            std::cout << std::endl;
        }else{
            std::cout << "GradeTable(" << get_y_size() << ", " << get_x_size() << ")" << std::endl;
            std::cout << "x_coords: ";
            for (const auto& y : y_coords) {
                std::cout << y << " ";
            }
            std::cout << std::endl;
            std::cout << "y_coords: ";
            for (const auto& x : x_coords) {
                std::cout << x << " ";
            }
            std::cout << std::endl;
        }
    }
};


// Sort and count betti result
// e.g. betti_result = [(0, 1), (0, 2), (0, 1), (1, 1), (1, 2), (1, 1)]
// output = [(0, 1, 2), (0, 2, 1), (1, 1, 2), (1, 2, 1)]
std::vector<std::tuple<int, int, int>> sort_count_betti_result(std::vector<std::pair<int, int>>& betti_result){
    
    std::map<std::pair<int, int>, int> count_map;

    // Step 1: Count occurrences
    for (const auto& p : betti_result) {
        count_map[p]++;
    }

    // Step 2: Convert to vector of tuples
    std::vector<std::tuple<int, int, int>> result;
    for (const auto& [key, count] : count_map) {
        result.emplace_back(key.first, key.second, count);
    }
    return result;
}
