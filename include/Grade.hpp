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
/*
    It is used to efficiently store and iterate over grades on a grid
    Summary:
        GradePoint: a point in the grid of grade Table with a partial order 
        GradeTable: collection of GradePoints stored in the lexicographic order
        GradeData: data associated with each grade point

    Iteration usage of GradeTable:
        for (const auto& gp : grade_table) {
            std::cout << gp << std::endl;
        }
*/

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
        return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y);
    }
};

template <typename VTX, typename VTY> // Coordinate Value Type (double often) of x and y
class GradeTable {
private:
    
    // Sets of x and y coordinates for discretization
    std::set<VTX> x_coords; // sorted x coordinates
    std::set<VTY> y_coords; // sorted y coordinates

public:
    // Default constructor
    GradeTable() = default;
    
    // Constructor by x_coords and y_coords
    GradeTable(const std::set<VTX>& x_coords, const std::set<VTY>& y_coords) 
        : x_coords(x_coords), y_coords(y_coords) {}

    // Get index of x coordinate (expensive operation O(log(n)))
    VTX get_x_value(int x_rank) const {
        auto it = std::next(x_coords.begin(), x_rank);
        if (it == x_coords.end()) {
            throw std::runtime_error("Error: x rank " + std::to_string(x_rank) + " coordinate not found in GradeTable");
        }
        return *it;
    }

    // Get index of y coordinate (expensive operation O(log(n)))
    VTY get_y_value(int y_rank) const {
        auto it = std::next(y_coords.begin(), y_rank);
        if (it == y_coords.end()) {
            throw std::runtime_error("Error: y rank " + std::to_string(y_rank) + " coordinate not found in GradeTable");
        }
        return *it;
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

// /*
//     GradeData: data associated with each grade point used to store 
//         1. betti numbers
//         2. vertices and edges
    
//     Usage:
//         GradeData<VTX, VTY, int> grade_betti_0(grade_table);
//         grade_betti_0[grade_point] = 0;
// */
// template<typename VTX, typename VTY, typename T>
// class GradeData {
// private:
//     // Reference to the grade table
//     const GradeTable<VTX, VTY>& grade_table;
    
//     // Vector for data storage
//     std::vector<T> data;
    
//     // Default value for uninitialized data
//     T default_value;

// public:
//     // Constructor
//     GradeData(const GradeTable<VTX, VTY>& grade_table_) 
//         : grade_table(grade_table_), default_value() {
//         // Initialize vector with size of grade points
//         data.resize(grade_table_.size(), default_value);
//     }

//     // Constructor with initial value
//     GradeData(const GradeTable<VTX, VTY>& grade_table_, const T& initVal) 
//         : grade_table(grade_table_), default_value(initVal) {
//         // Initialize vector with size of grade points and initial value
//         data.resize(grade_table_.size(), initVal);
//     }

//     // Access operator using iterator
//     T& operator[](GPIter it) {
//         return data[(*it)->get_index()];
//     }

//     // Const access operator using iterator
//     const T& operator[](GPIter it) const {
//         return data[(*it)->get_index()];
//     }

//     // Access operator using const iterator
//     T& operator[](GPCIter it) {
//         return data[(*it)->get_index()];
//     }

//     // Const access operator using const iterator
//     const T& operator[](GPCIter it) const {
//         return data[(*it)->get_index()];
//     }
    
//     // Access operator using std::weak_ptr<GradePoint>
//     T& operator[](const std::weak_ptr<GradePoint>& grade_ptr) {
//         if (auto shared_ptr = grade_ptr.lock()) {
//             return data[shared_ptr->get_index()];
//         }
//         throw std::runtime_error("Weak pointer expired");
//     }
    
//     // Const access operator using std::weak_ptr<GradePoint>
//     const T& operator[](const std::weak_ptr<GradePoint>& grade_ptr) const {
//         if (auto shared_ptr = grade_ptr.lock()) {
//             return data[shared_ptr->get_index()];
//         }
//         throw std::runtime_error("Weak pointer expired");
//     }
    
//     // Get the default value
//     const T& get_default_value() const {
//         return default_value;
//     }
    
//     // Set the default value
//     void set_default_value(const T& value) {
//         default_value = value;
//     }
    
//     // Get the size of the data
//     size_t size() const {
//         return data.size();
//     }
    
//     // Get data for a grade point
//     std::optional<T> get_data(const GradePoint& point) const {
//         auto it = grade_table.find(point);
//         if (it != grade_table.end()) {
//             return data[it->get_index()];
//         }
//         return std::nullopt;
//     }
    
//     // Print data
//     void print(bool print_default_value = false) const 
//     {
//         for (const auto& gp_ptr : grade_table) {
//             if (print_default_value){
//                 std::cout << gp_ptr->print() << ": " << data[gp_ptr] << std::endl;
//             }else{
//                 if (data[gp_ptr] != default_value){
//                     std::cout << gp_ptr->print() << ": " << data[gp_ptr] << std::endl;
//                 }
//             }
//         }
//     }
// };