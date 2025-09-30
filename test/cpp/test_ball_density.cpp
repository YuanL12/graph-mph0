#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "GGraph.hpp"
#include "Grade.hpp"
#include "IO.hpp"
#include "MPH.hpp"
#include "PointCloud.hpp"

// Helper function to compare two GGraph objects
bool compare_ggraphs(const GGraph& g1, const GGraph& g2) {
    // Compare number of vertices and edges
    if (g1.get_nvertices() != g2.get_nvertices()) {
        std::cout << "Different number of vertices: " << g1.get_nvertices() << " vs "
                  << g2.get_nvertices() << std::endl;
        return false;
    }

    if (g1.get_nedges() != g2.get_nedges()) {
        std::cout << "Different number of edges: " << g1.get_nedges() << " vs " << g2.get_nedges()
                  << std::endl;
        return false;
    }

    // Compare vertex grades
    const auto& vertices1 = g1.get_gvertices();
    const auto& vertices2 = g2.get_gvertices();

    for (size_t i = 0; i < vertices1.size(); ++i) {
        if (vertices1[i].get_grade() != vertices2[i].get_grade()) {
            std::cout << "Different vertex grade at index " << i << ": " << vertices1[i].get_grade()
                      << " vs " << vertices2[i].get_grade() << std::endl;
            return false;
        }
    }

    // Compare edge grades
    const auto& edges1 = g1.get_gedges();
    const auto& edges2 = g2.get_gedges();

    for (size_t i = 0; i < edges1.size(); ++i) {
        if (edges1[i].get_grade() != edges2[i].get_grade()) {
            std::cout << "Different edge grade at index " << i << ": " << edges1[i].get_grade()
                      << " vs " << edges2[i].get_grade() << std::endl;
            return false;
        }
    }

    return true;
}

// Helper function to compare two GradeTable objects
template <typename VTX, typename VTY>
bool compare_grade_tables(const GradeTable<VTX, VTY>& gt1, const GradeTable<VTX, VTY>& gt2) {
    // Compare sizes
    if (gt1.get_x_size() != gt2.get_x_size()) {
        std::cout << "Different x coordinate size: " << gt1.get_x_size() << " vs "
                  << gt2.get_x_size() << std::endl;
        return false;
    }

    if (gt1.get_y_size() != gt2.get_y_size()) {
        std::cout << "Different y coordinate size: " << gt1.get_y_size() << " vs "
                  << gt2.get_y_size() << std::endl;
        return false;
    }

    // Compare x coordinates
    auto x_coords1 = gt1.get_x_coords();
    auto x_coords2 = gt2.get_x_coords();

    for (size_t i = 0; i < x_coords1.size(); ++i) {
        if (std::abs(x_coords1[i] - x_coords2[i]) > 1e-10) {
            std::cout << "Different x coordinate at index " << i << ": " << std::setprecision(15)
                      << x_coords1[i] << " vs " << x_coords2[i] << std::endl;
            return false;
        }
    }

    // Compare y coordinates
    auto y_coords1 = gt1.get_y_coords();
    auto y_coords2 = gt2.get_y_coords();

    for (size_t i = 0; i < y_coords1.size(); ++i) {
        if (std::abs(y_coords1[i] - y_coords2[i]) > 1e-10) {
            std::cout << "Different y coordinate at index " << i << ": " << std::setprecision(15)
                      << y_coords1[i] << " vs " << y_coords2[i] << std::endl;
            return false;
        }
    }

    return true;
}

// Helper function to compare MPH0 results
bool compare_mph0_results(const std::vector<std::pair<int, int>>& betti_0_1,
                          const std::vector<std::pair<int, int>>& betti_1_1,
                          const std::vector<std::pair<int, int>>& betti_2_1,
                          const std::vector<std::pair<int, int>>& betti_0_1_1,
                          const std::vector<std::tuple<size_t, size_t, int>>& M1,
                          const std::vector<std::pair<int, int>>& betti_0_2,
                          const std::vector<std::pair<int, int>>& betti_1_2,
                          const std::vector<std::pair<int, int>>& betti_2_2,
                          const std::vector<std::pair<int, int>>& betti_0_1_2,
                          const std::vector<std::tuple<size_t, size_t, int>>& M2,
                          const std::string& test_name) {
    bool all_equal = true;

    // Compare betti_0
    if (betti_0_1 != betti_0_2) {
        std::cout << "Different betti_0 in " << test_name << ": ";
        std::cout << "Size " << betti_0_1.size() << " vs " << betti_0_2.size() << std::endl;
        all_equal = false;
    }

    // Compare betti_1
    if (betti_1_1 != betti_1_2) {
        std::cout << "Different betti_1 in " << test_name << ": ";
        std::cout << "Size " << betti_1_1.size() << " vs " << betti_1_2.size() << std::endl;
        all_equal = false;
    }

    // Compare betti_2
    if (betti_2_1 != betti_2_2) {
        std::cout << "Different betti_2 in " << test_name << ": ";
        std::cout << "Size " << betti_2_1.size() << " vs " << betti_2_2.size() << std::endl;
        all_equal = false;
    }

    // Compare betti_0_1
    if (betti_0_1_1 != betti_0_1_2) {
        std::cout << "Different betti_0_1 in " << test_name << ": ";
        std::cout << "Size " << betti_0_1_1.size() << " vs " << betti_0_1_2.size() << std::endl;
        all_equal = false;
    }

    // Compare matrix M (sparse matrix representation)
    if (M1 != M2) {
        std::cout << "Different matrix M in " << test_name << ": ";
        std::cout << "Size " << M1.size() << " vs " << M2.size() << std::endl;
        all_equal = false;
    }

    return all_equal;
}

// Test function for ball density Rips filtrations
int test_ball_density_filtrations() {
    std::cout << "Testing ball density Rips filtrations..." << std::endl;

    // Create a simple test point cloud (3 points in 2D)
    std::vector<std::vector<double>> points = {
        {0.0, 0.0},  // Point 0
        {1.0, 0.0},  // Point 1
        {0.5, 1.0}   // Point 2
    };

    std::cout << "Test point cloud:" << std::endl;
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "Point " << i << ": (" << points[i][0] << ", " << points[i][1] << ")"
                  << std::endl;
    }

    // Test with default radius threshold (20% of distances)
    std::cout << "\n=== Testing with default radius threshold ===" << std::endl;

    auto [ggraph1, grade_table1] = point_cloud_to_ball_density_Rips_filtration(points);
    auto [ggraph2, grade_table2] = point_cloud_to_ball_density_Rips_filtration_v2(points);

    std::cout << "GGraph 1 - Vertices: " << ggraph1.get_nvertices()
              << ", Edges: " << ggraph1.get_nedges() << std::endl;
    std::cout << "GGraph 2 - Vertices: " << ggraph2.get_nvertices()
              << ", Edges: " << ggraph2.get_nedges() << std::endl;

    std::cout << "GradeTable 1 - X size: " << grade_table1.get_x_size()
              << ", Y size: " << grade_table1.get_y_size() << std::endl;
    std::cout << "GradeTable 2 - X size: " << grade_table2.get_x_size()
              << ", Y size: " << grade_table2.get_y_size() << std::endl;

    // Compare results
    bool ggraph_equal = compare_ggraphs(ggraph1, ggraph2);
    bool grade_table_equal = compare_grade_tables(grade_table1, grade_table2);

    std::cout << "GGraph comparison: " << (ggraph_equal ? "PASS" : "FAIL") << std::endl;
    std::cout << "GradeTable comparison: " << (grade_table_equal ? "PASS" : "FAIL") << std::endl;

    // Test MPH0 computation
    std::cout << "\n--- Testing MPH0 computation (default threshold) ---" << std::endl;
    auto [betti_0_1, betti_1_1, betti_2_1, betti_0_1_1, M1] = compute_MPH0(ggraph1);
    auto [betti_0_2, betti_1_2, betti_2_2, betti_0_1_2, M2] = compute_MPH0(ggraph2);

    std::cout << "MPH0 Results 1 - betti_0: " << betti_0_1.size()
              << ", betti_1: " << betti_1_1.size() << ", betti_2: " << betti_2_1.size()
              << ", betti_0_1: " << betti_0_1_1.size() << ", M: " << M1.size() << std::endl;
    std::cout << "MPH0 Results 2 - betti_0: " << betti_0_2.size()
              << ", betti_1: " << betti_1_2.size() << ", betti_2: " << betti_2_2.size()
              << ", betti_0_1: " << betti_0_1_2.size() << ", M: " << M2.size() << std::endl;

    bool mph0_equal =
        compare_mph0_results(betti_0_1, betti_1_1, betti_2_1, betti_0_1_1, M1, betti_0_2, betti_1_2,
                             betti_2_2, betti_0_1_2, M2, "default threshold");
    std::cout << "MPH0 comparison (default threshold): " << (mph0_equal ? "PASS" : "FAIL")
              << std::endl;

    // Test with explicit radius threshold
    std::cout << "\n=== Testing with explicit radius threshold (0.5) ===" << std::endl;

    double radius_threshold = 0.5;
    auto [ggraph3, grade_table3] =
        point_cloud_to_ball_density_Rips_filtration(points, radius_threshold);
    auto [ggraph4, grade_table4] =
        point_cloud_to_ball_density_Rips_filtration_v2(points, radius_threshold);

    std::cout << "GGraph 3 - Vertices: " << ggraph3.get_nvertices()
              << ", Edges: " << ggraph3.get_nedges() << std::endl;
    std::cout << "GGraph 4 - Vertices: " << ggraph4.get_nvertices()
              << ", Edges: " << ggraph4.get_nedges() << std::endl;

    std::cout << "GradeTable 3 - X size: " << grade_table3.get_x_size()
              << ", Y size: " << grade_table3.get_y_size() << std::endl;
    std::cout << "GradeTable 4 - X size: " << grade_table4.get_x_size()
              << ", Y size: " << grade_table4.get_y_size() << std::endl;

    // Compare results with explicit threshold
    bool ggraph_equal_explicit = compare_ggraphs(ggraph3, ggraph4);
    bool grade_table_equal_explicit = compare_grade_tables(grade_table3, grade_table4);

    std::cout << "GGraph comparison (explicit threshold): "
              << (ggraph_equal_explicit ? "PASS" : "FAIL") << std::endl;
    std::cout << "GradeTable comparison (explicit threshold): "
              << (grade_table_equal_explicit ? "PASS" : "FAIL") << std::endl;

    // Test MPH0 computation with explicit threshold
    std::cout << "\n--- Testing MPH0 computation (explicit threshold) ---" << std::endl;
    auto [betti_0_3, betti_1_3, betti_2_3, betti_0_1_3, M3] = compute_MPH0(ggraph3);
    auto [betti_0_4, betti_1_4, betti_2_4, betti_0_1_4, M4] = compute_MPH0(ggraph4);

    std::cout << "MPH0 Results 3 - betti_0: " << betti_0_3.size()
              << ", betti_1: " << betti_1_3.size() << ", betti_2: " << betti_2_3.size()
              << ", betti_0_1: " << betti_0_1_3.size() << ", M: " << M3.size() << std::endl;
    std::cout << "MPH0 Results 4 - betti_0: " << betti_0_4.size()
              << ", betti_1: " << betti_1_4.size() << ", betti_2: " << betti_2_4.size()
              << ", betti_0_1: " << betti_0_1_4.size() << ", M: " << M4.size() << std::endl;

    bool mph0_equal_explicit =
        compare_mph0_results(betti_0_3, betti_1_3, betti_2_3, betti_0_1_3, M3, betti_0_4, betti_1_4,
                             betti_2_4, betti_0_1_4, M4, "explicit threshold");
    std::cout << "MPH0 comparison (explicit threshold): " << (mph0_equal_explicit ? "PASS" : "FAIL")
              << std::endl;

    // Test with larger point cloud
    std::cout << "\n=== Testing with larger point cloud (4 points) ===" << std::endl;

    std::vector<std::vector<double>> points_large = {
        {0.0, 0.0},  // Point 0
        {1.0, 0.0},  // Point 1
        {0.5, 1.0},  // Point 2
        {0.5, 0.5}   // Point 3
    };

    auto [ggraph5, grade_table5] = point_cloud_to_ball_density_Rips_filtration(points_large);
    auto [ggraph6, grade_table6] = point_cloud_to_ball_density_Rips_filtration_v2(points_large);

    std::cout << "GGraph 5 - Vertices: " << ggraph5.get_nvertices()
              << ", Edges: " << ggraph5.get_nedges() << std::endl;
    std::cout << "GGraph 6 - Vertices: " << ggraph6.get_nvertices()
              << ", Edges: " << ggraph6.get_nedges() << std::endl;

    bool ggraph_equal_large = compare_ggraphs(ggraph5, ggraph6);
    bool grade_table_equal_large = compare_grade_tables(grade_table5, grade_table6);

    std::cout << "GGraph comparison (large point cloud): " << (ggraph_equal_large ? "PASS" : "FAIL")
              << std::endl;
    std::cout << "GradeTable comparison (large point cloud): "
              << (grade_table_equal_large ? "PASS" : "FAIL") << std::endl;

    // Test MPH0 computation with larger point cloud
    std::cout << "\n--- Testing MPH0 computation (large point cloud) ---" << std::endl;
    auto [betti_0_5, betti_1_5, betti_2_5, betti_0_1_5, M5] = compute_MPH0(ggraph5);
    auto [betti_0_6, betti_1_6, betti_2_6, betti_0_1_6, M6] = compute_MPH0(ggraph6);

    std::cout << "MPH0 Results 5 - betti_0: " << betti_0_5.size()
              << ", betti_1: " << betti_1_5.size() << ", betti_2: " << betti_2_5.size()
              << ", betti_0_1: " << betti_0_1_5.size() << ", M: " << M5.size() << std::endl;
    std::cout << "MPH0 Results 6 - betti_0: " << betti_0_6.size()
              << ", betti_1: " << betti_1_6.size() << ", betti_2: " << betti_2_6.size()
              << ", betti_0_1: " << betti_0_1_6.size() << ", M: " << M6.size() << std::endl;

    bool mph0_equal_large =
        compare_mph0_results(betti_0_5, betti_1_5, betti_2_5, betti_0_1_5, M5, betti_0_6, betti_1_6,
                             betti_2_6, betti_0_1_6, M6, "large point cloud");
    std::cout << "MPH0 comparison (large point cloud): " << (mph0_equal_large ? "PASS" : "FAIL")
              << std::endl;

    // Test with real data from annulus_200.txt
    std::cout << "\n=== Testing with real data (annulus_200.txt) ===" << std::endl;

    bool ggraph_equal_real = true;
    bool grade_table_equal_real = true;
    bool mph0_equal_real = true;

    try {
        // Read points from the annulus_200.txt file
        std::string filename =
            "/home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt";
        auto points_real = read_points<double>(filename);

        std::cout << "Loaded " << points_real.size() << " points from " << filename << std::endl;
        std::cout << "First few points:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), points_real.size()); ++i) {
            std::cout << "Point " << i << ": (" << points_real[i][0] << ", " << points_real[i][1]
                      << ")" << std::endl;
        }

        // Test with default radius threshold
        std::cout << "\n--- Testing with default radius threshold ---" << std::endl;
        auto [ggraph_real1, grade_table_real1] =
            point_cloud_to_ball_density_Rips_filtration(points_real);
        auto [ggraph_real2, grade_table_real2] =
            point_cloud_to_ball_density_Rips_filtration_v2(points_real);

        std::cout << "GGraph Real 1 - Vertices: " << ggraph_real1.get_nvertices()
                  << ", Edges: " << ggraph_real1.get_nedges() << std::endl;
        std::cout << "GGraph Real 2 - Vertices: " << ggraph_real2.get_nvertices()
                  << ", Edges: " << ggraph_real2.get_nedges() << std::endl;

        std::cout << "GradeTable Real 1 - X size: " << grade_table_real1.get_x_size()
                  << ", Y size: " << grade_table_real1.get_y_size() << std::endl;
        std::cout << "GradeTable Real 2 - X size: " << grade_table_real2.get_x_size()
                  << ", Y size: " << grade_table_real2.get_y_size() << std::endl;

        // Compare GGraph and GradeTable
        ggraph_equal_real = compare_ggraphs(ggraph_real1, ggraph_real2);
        grade_table_equal_real = compare_grade_tables(grade_table_real1, grade_table_real2);

        std::cout << "GGraph comparison (real data): " << (ggraph_equal_real ? "PASS" : "FAIL")
                  << std::endl;
        std::cout << "GradeTable comparison (real data): "
                  << (grade_table_equal_real ? "PASS" : "FAIL") << std::endl;

        // Test MPH0 computation with real data
        std::cout << "\n--- Testing MPH0 computation (real data) ---" << std::endl;
        auto [betti_0_real1, betti_1_real1, betti_2_real1, betti_0_1_real1, M_real1] =
            compute_MPH0(ggraph_real1);
        auto [betti_0_real2, betti_1_real2, betti_2_real2, betti_0_1_real2, M_real2] =
            compute_MPH0(ggraph_real2);

        std::cout << "MPH0 Results Real 1 - betti_0: " << betti_0_real1.size()
                  << ", betti_1: " << betti_1_real1.size() << ", betti_2: " << betti_2_real1.size()
                  << ", betti_0_1: " << betti_0_1_real1.size() << ", M: " << M_real1.size()
                  << std::endl;
        std::cout << "MPH0 Results Real 2 - betti_0: " << betti_0_real2.size()
                  << ", betti_1: " << betti_1_real2.size() << ", betti_2: " << betti_2_real2.size()
                  << ", betti_0_1: " << betti_0_1_real2.size() << ", M: " << M_real2.size()
                  << std::endl;

        mph0_equal_real = compare_mph0_results(
            betti_0_real1, betti_1_real1, betti_2_real1, betti_0_1_real1, M_real1, betti_0_real2,
            betti_1_real2, betti_2_real2, betti_0_1_real2, M_real2, "real data");
        std::cout << "MPH0 comparison (real data): " << (mph0_equal_real ? "PASS" : "FAIL")
                  << std::endl;

        // Show some sample Betti numbers for verification
        if (!betti_0_real1.empty()) {
            std::cout << "Sample betti_0 values (first 3): ";
            for (size_t i = 0; i < std::min(size_t(3), betti_0_real1.size()); ++i) {
                std::cout << "(" << betti_0_real1[i].first << "," << betti_0_real1[i].second
                          << ") ";
            }
            std::cout << std::endl;
        }

    } catch (const std::exception& e) {
        std::cout << "ERROR: Failed to test with real data: " << e.what() << std::endl;
        std::cout << "Skipping real data tests..." << std::endl;
        ggraph_equal_real = false;
        grade_table_equal_real = false;
        mph0_equal_real = false;
    }

    // Overall result
    bool all_tests_passed = ggraph_equal && grade_table_equal && mph0_equal &&
                            ggraph_equal_explicit && grade_table_equal_explicit &&
                            mph0_equal_explicit && ggraph_equal_large && grade_table_equal_large &&
                            mph0_equal_large && ggraph_equal_real && grade_table_equal_real &&
                            mph0_equal_real;

    std::cout << "\n=== FINAL RESULT ===" << std::endl;
    std::cout << "All tests passed: " << (all_tests_passed ? "YES" : "NO") << std::endl;

    if (all_tests_passed) {
        std::cout << "SUCCESS: Both functions produce identical results!" << std::endl;
        std::cout << "All GGraph, GradeTable, and MPH0 comparisons passed!" << std::endl;
        std::cout
            << "Tests included: synthetic data (3 & 4 points) and real data (annulus_200.txt)!"
            << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Functions produce different results!" << std::endl;
        return 1;
    }
}

int main() { return test_ball_density_filtrations(); }
