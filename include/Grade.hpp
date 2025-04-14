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
/*
    It is used to efficiently store and iterate over grades on a grid
    Summary:
        GradePoint: a point in the grid of grade Table with a partial order 
        GradeTable: collection of GradePoints stored in the lexicographic order
        GradeData: data associated with each grade point
*/

// GradePoint: a point in the grid of grade Table with a partial order 
struct GradePoint {
    int x = -1; // index of x coordinate
    int y = -1; // index of y coordinate
    int index = -1; // index of grade point in the vector for iteration

    GradePoint(int x_, int y_, int index_) : x(x_), y(y_), index(index_) {}

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
    
    // Less than operator for sorting
    bool operator<(const GradePoint& other) const {
        return less_than(other);
    }

    // print operator
    friend std::ostream& operator<<(std::ostream& os, const GradePoint& gp) {
        os << "(" << gp.x << ", " << gp.y << ", [" << gp.index << "])";
        return os;
    }

    // print operator
    std::string print() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ", [" + std::to_string(index) + "])";
    }
};

// Iterator types
using GPIter = typename std::vector<GradePoint>::iterator;
using GPCIter = typename std::vector<GradePoint>::const_iterator;

template <typename VTX, typename VTY> // Coordinate Value Type (double often) of x and y
class GradeTable {
private:
    // Custom comparator for fuzzy comparison of floating point numbers
    struct FuzzyCompare {
        template<typename T>
        bool operator()(const T& a, const T& b) const {
            if constexpr (std::is_floating_point_v<T>) {
                return a < b && std::abs(a - b) >= 1e-10;
            } else {
                return a < b;
            }
        }
    };
    
    // Sets of x and y coordinates for discretization with fuzzy comparison
    std::set<VTX, FuzzyCompare> x_coords; // sorted x coordinates
    std::set<VTY, FuzzyCompare> y_coords; // sorted y coordinates

    // Vector of all grade points for iteration
    std::vector<GradePoint> grade_points;

public:
    // Default constructor
    GradeTable() = default;
    
    // Constructor with multiple points in R^2
    GradeTable(const std::vector<std::pair<VTX, VTY>>& points) {
        // Add coordinates if they don't exist
        for (const auto& point : points) {
            x_coords.insert(point.first);
            y_coords.insert(point.second);
        }
        
        // Reserve space for grade points
        grade_points.reserve(points.size());
        
        // Create grade points directly in grade_points
        for (const auto& point : points) {
            int x_idx = std::distance(x_coords.begin(), x_coords.find(point.first));
            int y_idx = std::distance(y_coords.begin(), y_coords.find(point.second));
            grade_points.emplace_back(x_idx, y_idx, -1);
        }
        
        // Sort grade points by x and y coordinates (lexicographic order)
        std::sort(grade_points.begin(), grade_points.end(), 
            [](const GradePoint& a, const GradePoint& b) {
                return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y);
            });
        
        // Remove duplicates
        auto last = std::unique(grade_points.begin(), grade_points.end());
        grade_points.erase(last, grade_points.end());
        
        // Assign indices after sorting
        for (size_t i = 0; i < grade_points.size(); ++i) {
            grade_points[i].index = i;
        }
    }

    // Get index of x coordinate (expensive operation O(log(n)))
    int get_x_index(const VTX& x) const {
        auto it = x_coords.find(x);
        if (it != x_coords.end()) {
            return std::distance(x_coords.begin(), it);
        }
        return -1; // Not found
    }

    // Get index of y coordinate (expensive operation O(log(n)))
    int get_y_index(const VTY& y) const {
        auto it = y_coords.find(y);
        if (it != y_coords.end()) {
            return std::distance(y_coords.begin(), it);
        }
        return -1; // Not found
    }

    // Get grade value for a given grade point
    std::tuple<VTX, VTY> get_grade_value(const GradePoint& grade_point) const {
        auto x_it = std::next(x_coords.begin(), grade_point.x);
        auto y_it = std::next(y_coords.begin(), grade_point.y);
        return std::make_tuple(*x_it, *y_it);
    }   

    // Iterator methods
    GPIter begin() { return grade_points.begin(); }
    GPIter end() { return grade_points.end(); }
    GPCIter begin() const { return grade_points.begin(); }
    GPCIter end() const { return grade_points.end(); }
    GPCIter cbegin() const { return grade_points.cbegin(); }
    GPCIter cend() const { return grade_points.cend(); }

    // Get total number of grade points
    size_t size() const { return grade_points.size(); }

    // Get number of x coordinates
    size_t get_x_size() const { return x_coords.size(); }

    // Get number of y coordinates
    size_t get_y_size() const { return y_coords.size(); }
    
    // Check if a grade point exists
    bool has_grade_point(const VTX& x_val, const VTY& y_val) const {
        int x_idx = get_x_index(x_val);
        int y_idx = get_y_index(y_val);
        
        if (x_idx == -1 || y_idx == -1) {
            return false;
        }
        
        // Create a temporary point to search for
        GradePoint temp(x_idx, y_idx, -1);
        
        // Check if the point exists in the vector
        return std::find(grade_points.begin(), grade_points.end(), temp) != grade_points.end();
    }
    
    // Find a grade point
    GPIter find(const GradePoint& point) {
        return std::find(grade_points.begin(), grade_points.end(), point);
    }
    
    // Find a grade point (const version)
    GPCIter find(const GradePoint& point) const {
        return std::find(grade_points.begin(), grade_points.end(), point);
    }
};



/*
    GradeData: data associated with each grade point used to store 
        1. betti numbers
        2. vertices and edges
*/
template<typename VTX, typename VTY, typename T>
class GradeData {
private:
    // Reference to the grade table
    const GradeTable<VTX, VTY>& grade_table;
    
    // Vector for data storage
    std::vector<T> data;
    
    // Default value for uninitialized data
    T default_value;

public:
    // Constructor
    GradeData(const GradeTable<VTX, VTY>& grade_table_) 
        : grade_table(grade_table_), default_value() {
        // Initialize vector with size of grade points
        data.resize(grade_table_.size(), default_value);
    }

    // Constructor with initial value
    GradeData(const GradeTable<VTX, VTY>& grade_table_, const T& initVal) 
        : grade_table(grade_table_), default_value(initVal) {
        // Initialize vector with size of grade points and initial value
        data.resize(grade_table_.size(), initVal);
    }

    // Access operator using iterator
    T& operator[](GPIter it) {
        return data[it->index];
    }

    // Const access operator using iterator
    const T& operator[](GPIter it) const {
        return data[it->index];
    }

    // Access operator using const iterator
    T& operator[](GPCIter it) {
        return data[it->index];
    }

    // Const access operator using const iterator
    const T& operator[](GPCIter it) const {
        return data[it->index];
    }
    
    // Get the default value
    const T& get_default_value() const {
        return default_value;
    }
    
    // Set the default value
    void set_default_value(const T& value) {
        default_value = value;
    }
    
    // Get the size of the data
    size_t size() const {
        return data.size();
    }
    
    // Get data for a grade point
    std::optional<T> get_data(const GradePoint& point) const {
        auto it = grade_table.find(point);
        if (it != grade_table.end()) {
            return data[it->index];
        }
        return std::nullopt;
    }
    
    // Set data for a grade point
    void set_data(const GradePoint& point, const T& value) {
        auto it = grade_table.find(point);
        if (it != grade_table.end()) {
            data[it->index] = value;
        }else{
            std::cout << "Error: Grade point " << point << " NOT FOUND" << std::endl;
        }
        
    }
};