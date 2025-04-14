#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "Grade.hpp"

// Helper function to print a GradePoint
void print_grade_point(const GradePoint& gp) {
    std::cout << "(" << gp.x << ", " << gp.y << ") [idx: " << gp.index << "]";
}

// Helper function to print a vector of GradePoints
void print_grade_points(const std::vector<GradePoint>& points) {
    std::cout << "[";
    for (size_t i = 0; i < points.size(); ++i) {
        print_grade_point(points[i]);
        if (i < points.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

// Test GradePoint comparison
void test_grade_point_comparison() {
    std::cout << "Testing GradePoint comparison..." << std::endl;
    
    GradePoint p1(1, 2, 0);
    GradePoint p2(1, 3, 1);
    GradePoint p3(2, 1, 2);
    GradePoint p4(1, 2, 3); // Same as p1 but different index
    
    std::cout << "p1: "; print_grade_point(p1); std::cout << std::endl;
    std::cout << "p2: "; print_grade_point(p2); std::cout << std::endl;
    std::cout << "p3: "; print_grade_point(p3); std::cout << std::endl;
    std::cout << "p4: "; print_grade_point(p4); std::cout << std::endl;
    
    std::cout << "p1 < p2: " << (p1 < p2) << std::endl;
    std::cout << "p1 < p3: " << (p1 < p3) << std::endl;
    std::cout << "p3 < p1: " << (p3 < p1) << std::endl;
    std::cout << "p1 == p4: " << (p1 == p4) << std::endl;
    
    std::cout << std::endl;
}

// Test GradeTable with integer coordinates
void test_grade_table_int() {
    std::cout << "Testing GradeTable with integer coordinates..." << std::endl;
    
    // Create a vector of points
    std::vector<std::pair<int, int>> points = {
        {1, 2}, {3, 4}, {1, 2}, {2, 3}, {3, 4}, {4, 1}
    };
    
    // Create a GradeTable
    GradeTable<int, int> table(points);
    
    // Print the grade points
    std::cout << "Grade points: ";
    for (const auto& gp : table) {
        print_grade_point(gp);
        std::cout << " ";
    }
    std::cout << std::endl;
    
    // Test get_grade_value
    for (const auto& gp : table) {
        auto [x, y] = table.get_grade_value(gp);
        std::cout << "Grade point "; print_grade_point(gp);
        std::cout << " has value (" << x << ", " << y << ")" << std::endl;
    }
    
    // Test has_grade_point
    std::cout << "has_grade_point(1, 2): " << table.has_grade_point(1, 2) << std::endl;
    std::cout << "has_grade_point(5, 5): " << table.has_grade_point(5, 5) << std::endl;
    
    std::cout << std::endl;
}

// Test GradeTable with double coordinates (fuzzy comparison)
void test_grade_table_double() {
    std::cout << "Testing GradeTable with double coordinates (fuzzy comparison)..." << std::endl;
    
    // Create a vector of points with some close values
    std::vector<std::pair<double, double>> points = {
        {1.1, 2.0}, 
        {3.0, 4.0}, 
        {1.1, 2.0}, // Duplicate
        {1.0, 2.0}, // Duplicate
        {1.1 + 1e-11, 2.0}, // Very close to (1.1, 2.0)
        {2.0, 3.0}, 
        {3.0, 4.0}, // Duplicate
        {4.2, 1.0}
    };
    
    // Create a GradeTable
    GradeTable<double, double> table(points);
    
    // Print the grade points
    std::cout << "Grade points: ";
    for (const auto& gp : table) {
        print_grade_point(gp);
        std::cout << " ";
    }
    std::cout << std::endl;
    
    // Test get_grade_value
    for (const auto& gp : table) {
        auto [x, y] = table.get_grade_value(gp);
        std::cout << "Grade point "; print_grade_point(gp);
        std::cout << " has value (" << x << ", " << y << ")" << std::endl;
    }
    
    // Test has_grade_point with fuzzy comparison
    std::cout << "has_grade_point(1.0, 2.0): " << table.has_grade_point(1.0, 2.0) << std::endl;
    std::cout << "has_grade_point(1.1, 2.0): " << table.has_grade_point(1.1, 2.0) << std::endl;
    std::cout << "has_grade_point(1.1 + 1e-11, 2.0): " << table.has_grade_point(1.1 + 1e-11, 2.0) << std::endl;
    std::cout << "has_grade_point(1.1 - 1e-11, 2.0): " << table.has_grade_point(1.1 - 1e-11, 2.0) << std::endl;
    std::cout << "has_grade_point(1.1 + 1e-9, 2.0): " << table.has_grade_point(1.1 + 1e-9, 2.0) << std::endl;
    std::cout << "has_grade_point(1.1 - 1e-9, 2.0): " << table.has_grade_point(1.1 - 1e-9, 2.0) << std::endl;
    
    std::cout << std::endl;
}

// Test GradeData
void test_grade_data() {
    std::cout << "Testing GradeData..." << std::endl;
    
    // Create a vector of points
    std::vector<std::pair<int, double>> points = {
        {1, 2.1}, {3, 4.3}, {2, 3.2}, {4, 1.4}
    };
    
    // Create a GradeTable
    GradeTable<int, double> table(points);
    
    // Create a GradeData with string values
    std::cout << "Creating GradeData with string Love..." << std::endl;
    GradeData<int, double, std::string> data(table, "Love");
    
    // Print the data
    std::cout << "GradeData values:" << std::endl;
    for (auto it = table.cbegin(); it != table.cend(); ++it) {
        std::cout << "Grade point " << *it;
        std::cout << " has data: " << data[it] << std::endl;
    }

    // Set some data
    std::cout << "Setting data for each grade point..." << std::endl;
    for (auto it = table.begin(); it != table.end(); ++it) {
        auto [x, y] = table.get_grade_value(*it);
        data[it] = "Point(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }
    
    // Print the data
    std::cout << "GradeData values:" << std::endl;
    for (auto it = table.begin(); it != table.end(); ++it) {
        std::cout << "Grade point " << *it;
        std::cout << " has data: " << data[it] << std::endl;
    }
    
    // Test get_data and set_data
    std::cout << "Testing get_data and set_data with non-existent grade point..." << std::endl;
    GradePoint gp(1, 1, -1); // This does not exist in the table
    std::cout << "get_data for " << gp;
    std::cout << ": " << data.get_data(gp).value_or("Not found") << std::endl;
    
    data.set_data(gp, "Updated value");
    std::cout << "After update, get_data for " << gp;
    std::cout << ": " << data.get_data(gp).value_or("Not found") << std::endl;

    // Test get_data and set_data with existing grade point
    std::cout << "Testing get_data and set_data with existing grade point..." << std::endl;
    GradePoint gp2(1, 2, -1); // This exists in the table
    std::cout << "get_data for " << gp2;
    std::cout << ": " << data.get_data(gp2).value_or("Not found") << std::endl;
    
    data.set_data(gp2, "Updated value");
    std::cout << "After update, get_data for " << gp2;
    std::cout << ": " << data.get_data(gp2).value_or("Not found") << std::endl;
    std::cout << std::endl;
    
}

int main() {
    test_grade_point_comparison();
    test_grade_table_int();
    test_grade_table_double();
    test_grade_data();
    
    std::cout << "All tests completed successfully!" << std::endl;
    return 0;
} 