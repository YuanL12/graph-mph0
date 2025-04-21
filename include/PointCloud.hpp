// Functions to construct a 1-critical filtration from a point cloud

#pragma once

#include <vector>
#include <cassert>
#include <iostream>
#include <map>
#include "GGraph.hpp"
#include <icecream.hpp>

// Construct a 1-critical filtration from a point cloud
// Input: a vector of points, each point is a vector of coordinates
// Output: a GGraph object and a GradeTable object
template<typename PT>
std::tuple<GGraph<PT, int>, GradeTable<PT, int>> point_cloud_to_degree_Rips_filtration(const std::vector<std::vector<PT>>& points){
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
    GGraph<PT, int> ggraph(n*n, vertex_grades, edges, edge_grades);
    return std::make_tuple(ggraph, grade_table);
}