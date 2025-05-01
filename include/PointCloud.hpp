/*
 * Functions to construct a 1-critical filtration from a point cloud
 * 
 * - 1. Degree-Rips filtration
 * 
 * - 2. Function-Rips filtration
 *   - 2.1. Ball-density Function-Rips filtration
 *   - 2.2. Gaussian density function (TODO)
 *   - 2.3. Eccentricity function (TODO)
 * 
 */

#pragma once

#include <vector>
#include <cassert>
#include <iostream>
#include <map>
#include "GGraph.hpp"
#include <icecream.hpp>
#include <optional>
// Construct the Degree-Rips filtration from a point cloud, it will be 1-critical by 
// inserting vertices and edges with multiplicity. 
// Input: a vector of points, each point is a vector of coordinates
// Output: a GGraph object and a GradeTable object
template<typename PT>
std::tuple<GGraph, GradeTable<PT, int>> point_cloud_to_degree_Rips_filtration(const std::vector<std::vector<PT>>& points){
    // Get the number of points
    const int n = points.size();

    // create a vector of indices [0, 1, 2, ..., n-1]
    std::vector<int> degrees(n);
    std::iota(degrees.begin(), degrees.end(), 0);

    // create a vector of all distances
    std::set<PT> all_distances;
    all_distances.insert(0);

    // Compute distance matrix 
    std::vector<std::vector<PT>> D(n, std::vector<PT>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // compute the distance between the two points
            PT dist = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k)
                dist += (points[i][k] - points[j][k]) * (points[i][k] - points[j][k]);

            PT dist_sqrt = std::sqrt(dist);
            // add the distance to the vector of all distances
            D[i][j] = dist_sqrt;
            D[j][i] = dist_sqrt;
            all_distances.insert(dist_sqrt);
        }
    }


    // Create a map of distances to indices (rank in radius coordinate)
    std::unordered_map<PT, int> distance_to_index;
    int index = 0;
    for (const auto& value : all_distances) {
        distance_to_index[value] = index;
        ++index;
    }

    // construct a GradeTable with x and y coordinates
    // x: distance, y: - degree
    std::set<int> negative_degrees;
    for (int i = 0; i < n; ++i) {
        negative_degrees.insert(-i);
    }
    GradeTable<PT, int> grade_table(all_distances, negative_degrees);
    // free the memory of all_distances and negative_degrees
    std::set<PT>().swap(all_distances);
    std::set<int>().swap(negative_degrees);
    
    // Create a copy of the distance matrix for sorting
    std::vector<std::vector<PT>> sorted_dists(n, std::vector<PT>(n));
    for (int i = 0; i < n; ++i) {
        sorted_dists[i] = D[i];
        std::sort(sorted_dists[i].begin(), sorted_dists[i].end());
    }

    // prepare for GGraph construction
    std::vector<std::pair<int, int>> edges; edges.reserve(n*n + n*(n-1)/2);
    std::vector<GradePoint> vertex_grades; vertex_grades.reserve(n*n);
    std::vector<GradePoint> edge_grades; edge_grades.reserve(n*n + n*(n-1)/2);

    // create vertex grades
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            Vertex v = i * n + j;
            int x_rank = distance_to_index[sorted_dists[i][j]];
            int y_rank = n - 1 - j; 
            vertex_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    // Add edges from the same row
    for (int i = 0; i < n; ++i) {
        int base = i * n;
        // Add edges from the same row e_j = (j, j+1), j = 0, 1, ..., n-2
        // f(e_j) = (r_{j+1} , -j) 
        for (int j = 0; j < n-1; ++j) {
            int x_rank = distance_to_index[sorted_dists[i][j + 1]];
            int y_rank = n - 1 - j;
            edges.emplace_back(base + j, base + j + 1);
            edge_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    // Add edges across vertices
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // compute max distance between i and j
            std::vector<PT> max_r(n);
            for (int k = 0; k < n; ++k)
                max_r[k] = std::max(sorted_dists[i][k], sorted_dists[j][k]);

            // find the first index k such that max_r[k] >= D[i][j]
            PT threshold = D[i][j];
            auto it = std::lower_bound(max_r.begin(), max_r.end(), threshold);
            int idx = it - max_r.begin();

            // add edges from i to j
            for (int k = idx; k < n; ++k) {
                // transform to 1D vertex index
                int ii = i * n + k;
                int jj = j * n + k;
                edges.emplace_back(ii, jj);
                int x_rank = distance_to_index[max_r[k]];
                int y_rank = n - 1 - k;
                edge_grades.emplace_back(GradePoint(x_rank, y_rank));
            }
        }
    }

    // Create a GGraph
    GGraph ggraph(n*n, vertex_grades, edges, edge_grades);

    return std::make_tuple(std::move(ggraph), std::move(grade_table));
}



/*
 * Construct a ball-density function-Rips filtration from a point cloud
 * Input: 
 *   - a vector of points, each point is a vector of coordinates
 *   - a radius threshold optional
 * Output: a GGraph object and a GradeTable object
 * 
 * The logic is as follows:
 * 1. Compute the distance matrix D
 * 2. Compute the ball density for each point
 * 3. Create a GradeTable with x and y coordinates
 * 4. Create a GGraph
 */
template<typename PT>
std::tuple<GGraph, GradeTable<double, PT>> 
point_cloud_to_ball_density_Rips_filtration(const std::vector<std::vector<PT>>& points, std::optional<double> radius_threshold = std::nullopt)
{
    const int n = points.size();
    const int num_edges = n * (n - 1) / 2;

    // Compute distances and store in compressed format
    std::vector<PT> distances;
    distances.reserve(num_edges);
    
    // Helper function to get index in compressed vector
    auto get_compressed_index = [n](int i, int j) {
        if (i > j) std::swap(i, j);
        return i * n - (i * (i + 1)) / 2 + (j - i - 1);
    };

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            PT dist = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k) {
                PT diff = points[i][k] - points[j][k];
                dist += diff * diff;
            }
            PT dist_sqrt = std::sqrt(dist);
            distances.push_back(dist_sqrt);
        }
    }

    // Create distance set from vector and add 0.0 for self-distances
    std::set<PT> distance_set(distances.begin(), distances.end());
    distance_set.insert(0.0);

    // Create distance to index map
    std::unordered_map<PT, int> distance_to_index;
    int index = 0;
    for (const auto& value : distance_set) {
        distance_to_index[value] = index++;
    }

    // Set radius threshold
    double radius_threshold_value;
    if (radius_threshold) {
        radius_threshold_value = *radius_threshold;
    } else {
        // use 20% of the distance set as the threshold if not provided
        size_t threshold_index = static_cast<size_t>(num_edges * 0.2);
        radius_threshold_value = *std::next(distance_set.begin(), threshold_index);
    }

    // Compute ball densities on each vertex
    std::vector<double> ball_densities(n, 0.0);
    int total_mass = 0;
    
    for (int i = 0; i < n; ++i) {
        int d = 1; // degree of the i-th point, 1 for itself
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                PT dist = distances[get_compressed_index(i, j)];
                if (dist <= radius_threshold_value) {
                    d++;
                }
            }
        }
        // negate for bottom-up filtration F^-1(-inf, a]
        ball_densities[i] = -static_cast<double>(d);
        total_mass += d;
    }

    // Normalize and create function value set
    for (int i = 0; i < n; ++i) {
        ball_densities[i] /= total_mass;
    }

    // Create a set of negative ball densities for y-coordinates 
    std::set<double> function_value_set(ball_densities.begin(), ball_densities.end());

    // Create a map of ball densities to indices (rank in ball density coordinate)
    std::unordered_map<double, int> ball_density_to_index;
    index = 0;
    for (const auto& value : function_value_set) {
        ball_density_to_index[value] = index++;
    }

    // Construct a GradeTable with
    // x-coordinate: function value, y-coordinate: distance
    GradeTable<double, PT> grade_table(function_value_set, distance_set);

    // free the memory of distance_set and function_value_set
    std::set<PT>().swap(distance_set);
    std::set<double>().swap(function_value_set);

    // Prepare for GGraph construction with exact sizes
    std::vector<std::pair<int, int>> edges;
    edges.reserve(num_edges);
    std::vector<GradePoint> vertex_grades(n);
    std::vector<GradePoint> edge_grades;
    edge_grades.reserve(num_edges);

    // Create vertex grades
    for (int i = 0; i < n; ++i) {
        int x_rank = ball_density_to_index[ball_densities[i]];
        int y_rank = 0;
        vertex_grades[i] = GradePoint(x_rank, y_rank);
    }

    // Create edge grades
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            edges.emplace_back(i, j);
            int i_rank = ball_density_to_index[ball_densities[i]];
            int j_rank = ball_density_to_index[ball_densities[j]];
            int x_rank = std::max(i_rank, j_rank);
            int y_rank = distance_to_index[distances[get_compressed_index(i, j)]];
            edge_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    return std::make_tuple(GGraph(n, vertex_grades, edges, edge_grades), std::move(grade_table));
}