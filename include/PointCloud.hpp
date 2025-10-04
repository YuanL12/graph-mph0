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

#include <cassert>
#include <icecream.hpp>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

#include "Compare.hpp"
#include "GGraph.hpp"
#include "Hash.hpp"
#include "RIVET.hpp"

// Construct the Degree-Rips filtration from a point cloud, it will be 1-critical by
// inserting vertices and edges with multiplicity.
// Input: a vector of points, each point is a vector of coordinates
// Output: a GGraph object and a GradeTable object
template <typename PT>
std::tuple<GGraph, GradeTable<PT, int>> point_cloud_to_degree_Rips_filtration(
    const std::vector<std::vector<PT>> &points) {
    // Get the number of points
    const int n = points.size();

    // create a vector of indices [0, 1, 2, ..., n-1]
    std::vector<int> degrees(n);
    std::iota(degrees.begin(), degrees.end(), 0);

    // create a vector of all distances
    // Use exact comparison to avoid numeric precision issues with large point clouds
    std::set<PT, Compare<PT, false>> all_distances;
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
    for (const auto &value : all_distances) {
        distance_to_index[value] = index;
        ++index;
    }

    // construct a GradeTable with x and y coordinates
    // x: distance, y: - degree
    std::set<int, Compare<int>> negative_degrees;
    for (int i = 0; i < n; ++i) {
        negative_degrees.insert(-i);
    }
    std::vector<PT> x_coords(all_distances.begin(), all_distances.end());
    std::vector<int> y_coords(negative_degrees.begin(), negative_degrees.end());
    GradeTable<PT, int> grade_table(x_coords, y_coords);

    // free the memory of all_distances and negative_degrees
    std::vector<PT>().swap(x_coords);
    std::vector<int>().swap(y_coords);
    std::set<PT, Compare<PT, false>>().swap(all_distances);
    std::set<int, Compare<int>>().swap(negative_degrees);

    // Create a copy of the distance matrix for sorting
    std::vector<std::vector<PT>> sorted_dists(n, std::vector<PT>(n));
    for (int i = 0; i < n; ++i) {
        sorted_dists[i] = D[i];
        std::sort(sorted_dists[i].begin(), sorted_dists[i].end());
    }

    // prepare for GGraph construction
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n * n + n * (n - 1) / 2);
    std::vector<GradePoint> vertex_grades;
    vertex_grades.reserve(n * n);
    std::vector<GradePoint> edge_grades;
    edge_grades.reserve(n * n + n * (n - 1) / 2);

    // create vertex grades
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int v = i * n + j;
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
        for (int j = 0; j < n - 1; ++j) {
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
            for (int k = 0; k < n; ++k) max_r[k] = std::max(sorted_dists[i][k], sorted_dists[j][k]);

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
    GGraph ggraph(n * n, vertex_grades, edges, edge_grades);

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
template <typename PT>
std::tuple<GGraph, GradeTable<double, PT>> point_cloud_to_ball_density_Rips_filtration(
    const std::vector<std::vector<PT>> &points,
    std::optional<double> radius_threshold = std::nullopt) {
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
    // Use exact comparison to avoid numeric precision issues with large point clouds
    std::set<PT, Compare<PT, false>> distance_set(distances.begin(), distances.end());
    distance_set.insert(0.0);

    // Create distance to index map
    std::unordered_map<PT, int> distance_to_index;
    int index = 0;
    for (const auto &value : distance_set) {
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
        int d = 1;  // degree of the i-th point, 1 for itself
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
    std::set<double, Compare<double>> function_value_set(ball_densities.begin(),
                                                         ball_densities.end());

    // Create a map of ball densities to indices (rank in ball density coordinate)
    std::unordered_map<double, int> ball_density_to_index;
    index = 0;
    for (const auto &value : function_value_set) {
        ball_density_to_index[value] = index++;
    }

    // Construct a GradeTable with
    // x-coordinate: function value, y-coordinate: distance
    std::vector<PT> x_coords(function_value_set.begin(), function_value_set.end());
    std::vector<PT> y_coords(distance_set.begin(), distance_set.end());
    GradeTable<double, PT> grade_table(x_coords, y_coords);

    // free the memory
    std::vector<PT>().swap(x_coords);
    std::vector<PT>().swap(y_coords);
    std::set<PT, Compare<PT, false>>().swap(distance_set);
    std::set<double, Compare<double>>().swap(function_value_set);

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

/*
 * Construct a ball-density function-Rips filtration from a point cloud
 * Input:
 *   - a vector of points, each point is a vector of coordinates
 *   - a vector of function values on vertices
 *   - optional function value threshold
 *   - optional max distance
 * Output:
 *  - a GGraph object
 *  - a GradeTable object
 *
 * The logic is as follows:
 * 1. Compute the distance matrix D
 * 2. Create a GradeTable with x and y coordinates
 * 3. Create a GGraph
 */
template <typename PT>
std::tuple<GGraph, GradeTable<double, PT>> point_cloud_to_function_Rips_filtration(
    const std::vector<std::vector<PT>> &points, const std::vector<double> &function_values,
    std::optional<double> function_value_threshold = std::nullopt,
    std::optional<PT> max_distance = std::nullopt) {
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
    // Use exact comparison to avoid numeric precision issues with large point clouds
    std::set<PT, Compare<PT, false>> distance_set(distances.begin(), distances.end());
    distance_set.insert(0.0);

    // Create distance to index map
    std::unordered_map<PT, int> distance_to_index;
    int index = 0;
    for (const auto &value : distance_set) {
        distance_to_index[value] = index++;
    }

    // Create a set of negative ball densities for y-coordinates
    std::set<double> function_value_set(function_values.begin(), function_values.end());

    // Create a map of ball densities to indices (rank in ball density coordinate)
    std::unordered_map<double, int> function_value_to_index;
    index = 0;
    for (const auto &value : function_value_set) {
        function_value_to_index[value] = index++;
    }

    // Construct a GradeTable with
    // x-coordinate: function value, y-coordinate: distance
    std::vector<double> x_coords(function_value_set.begin(), function_value_set.end());
    std::vector<PT> y_coords(distance_set.begin(), distance_set.end());
    GradeTable<double, PT> grade_table(x_coords, y_coords);

    // free the memory
    std::vector<double>().swap(x_coords);
    std::vector<PT>().swap(y_coords);
    std::set<PT, Compare<PT, false>>().swap(distance_set);
    std::set<double>().swap(function_value_set);

    // Prepare for GGraph construction with exact sizes
    std::vector<std::pair<int, int>> edges;
    edges.reserve(num_edges);
    std::vector<GradePoint> vertex_grades(n);
    std::vector<GradePoint> edge_grades;
    edge_grades.reserve(num_edges);

    // Create vertex grades
    for (int i = 0; i < n; ++i) {
        int x_rank = function_value_to_index[function_values[i]];
        int y_rank = 0;
        vertex_grades[i] = GradePoint(x_rank, y_rank);
    }

    // Create edge grades
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            edges.emplace_back(i, j);
            int i_rank = function_value_to_index[function_values[i]];
            int j_rank = function_value_to_index[function_values[j]];
            int x_rank = std::max(i_rank, j_rank);
            int y_rank = distance_to_index[distances[get_compressed_index(i, j)]];
            edge_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    return std::make_tuple(GGraph(n, vertex_grades, edges, edge_grades), std::move(grade_table));
}

template <typename PT>
std::tuple<GGraph, GradeTable<double, PT>> point_cloud_to_ball_density_Rips_filtration_v2(
    const std::vector<std::vector<PT>> &points,
    std::optional<double> radius_threshold = std::nullopt) {
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
    // Use exact comparison to avoid numeric precision issues with large point clouds
    std::set<PT, Compare<PT, false>> distance_set(distances.begin(), distances.end());
    distance_set.insert(0.0);

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
        int d = 1;  // degree of the i-th point, 1 for itself
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

    return point_cloud_to_function_Rips_filtration<PT>(points, ball_densities, radius_threshold);
}

/*
 * Construct a ball-density function-Rips filtration from a point cloud
 * It uses the ExactValue type in RIVET.
 * It is slower than the above double version, because the need to convert double to
 * rational number.
 *
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
template <typename PT>
std::tuple<GGraph, GradeTable<rivet::ExactValue, rivet::ExactValue>>
point_cloud_to_ball_density_Rips_filtration_rational(
    const std::vector<std::vector<PT>> &points,
    std::optional<double> radius_threshold = std::nullopt) {
    const int n = points.size();
    const int num_edges = n * (n - 1) / 2;

    // Compute distances and store in compressed format
    std::vector<rivet::ExactValue> distances;
    distances.reserve(num_edges);

    // Create distance set from vector and add 0.0 for self-distances
    rivet::ExactSet distance_set;
    distance_set.insert(rivet::ExactValue(0.0));

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
            rivet::exact dist_sqrt_exact = rivet::approx(dist_sqrt);
            distances.emplace_back(rivet::ExactValue(dist_sqrt_exact));
            distance_set.insert(distances.back());
        }
    }

    // Create distance to index map
    std::unordered_map<rivet::exact, int> distance_to_index;
    int index = 0;
    for (const auto &value : distance_set) {
        distance_to_index[value.exact_value] = index++;
    }

    // Set radius threshold
    double radius_threshold_value;
    if (radius_threshold) {
        radius_threshold_value = *radius_threshold;
    } else {
        // use 20% of the distance set as the threshold if not provided
        std::vector<double> sorted_distances;
        sorted_distances.reserve(num_edges + 1);
        sorted_distances.emplace_back(0.0);
        for (const auto &distance : distances) {
            sorted_distances.emplace_back(distance.double_value);
        }
        std::sort(sorted_distances.begin(), sorted_distances.end());
        size_t threshold_index = static_cast<size_t>(num_edges * 0.2);
        radius_threshold_value = sorted_distances[threshold_index];
    }

    // Compute ball densities on each vertex
    std::vector<double> ball_densities(n, 0.0);
    int total_mass = 0;

    for (int i = 0; i < n; ++i) {
        int d = 1;  // degree of the i-th point, 1 for itself
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                double dist = distances[get_compressed_index(i, j)].double_value;
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
    rivet::ExactSet function_value_set;
    for (int i = 0; i < n; ++i) {
        ball_densities[i] /= total_mass;
        // directly use the double precision value as the exact value
        // althogh different from the distance case, rivet use this convention.
        function_value_set.insert(rivet::ExactValue(ball_densities[i]));
    }

    // Create a map of ball densities to indices (rank in ball density coordinate)
    std::unordered_map<rivet::exact, int> ball_density_to_index;
    index = 0;
    for (const auto &value : function_value_set) {
        ball_density_to_index[value.exact_value] = index++;
    }

    // Construct a GradeTable with
    // x-coordinate: function value, y-coordinate: distance
    std::vector<rivet::ExactValue> x_coords(function_value_set.begin(), function_value_set.end());
    std::vector<rivet::ExactValue> y_coords(distance_set.begin(), distance_set.end());
    GradeTable<rivet::ExactValue, rivet::ExactValue> grade_table(x_coords, y_coords);

    // free the memory
    std::vector<rivet::ExactValue>().swap(x_coords);
    std::vector<rivet::ExactValue>().swap(y_coords);
    rivet::ExactSet().swap(distance_set);
    rivet::ExactSet().swap(function_value_set);

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
            int y_rank = distance_to_index[distances[get_compressed_index(i, j)].exact_value];
            edge_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    return std::make_tuple(GGraph(n, vertex_grades, edges, edge_grades), std::move(grade_table));
}

/*
 * Construct a Degree-Rips filtration from a point cloud using RIVET exact types
 * (x: degree, y: distance)
 */
template <typename PT>
std::tuple<GGraph, GradeTable<int, rivet::ExactValue>>
point_cloud_to_degree_Rips_filtration_rational(const std::vector<std::vector<PT>> &points) {
    const int n = points.size();
    const int num_edges = n * (n - 1) / 2;

    // Compute distances and store in compressed format
    std::vector<rivet::ExactValue> distances;
    distances.reserve(num_edges);

    // Create distance set from vector and add 0.0 for self-distances
    rivet::ExactSet distance_set;
    distance_set.insert(rivet::ExactValue(0.0));

    // Helper function to get index in compressed vector
    auto get_compressed_index = [n](int i, int j) {
        if (i > j) std::swap(i, j);
        return i * n - (i * (i + 1)) / 2 + (j - i - 1);
    };

    // Compute distance matrix
    std::vector<std::vector<rivet::ExactValue>> D(
        n, std::vector<rivet::ExactValue>(n, rivet::ExactValue(0.0)));
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            PT dist = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k) {
                PT diff = points[i][k] - points[j][k];
                dist += diff * diff;
            }
            PT dist_sqrt = std::sqrt(dist);
            rivet::exact dist_sqrt_exact = rivet::approx(dist_sqrt);
            rivet::ExactValue dist_sqrt_exact_value(dist_sqrt_exact);
            distances.emplace_back(dist_sqrt_exact_value);
            distance_set.insert(dist_sqrt_exact_value);

            // add the distance to the distance matrix
            D[i][j] = dist_sqrt_exact_value;
            D[j][i] = dist_sqrt_exact_value;
        }
    }

    // Create distance to index map
    std::unordered_map<rivet::exact, int> distance_to_index;
    int index = 0;
    for (const auto &value : distance_set) {
        distance_to_index[value.exact_value] = index++;
    }

    // Construct a GradeTable with
    // x-coordinate: negative degree, y-coordinate: distance
    std::vector<int> x_coords;  // -(n-1), -(n-2), ..., -1, 0
    for (int i = n - 1; i >= 0; --i) {
        x_coords.emplace_back(-i);
    }
    std::vector<rivet::ExactValue> y_coords(distance_set.begin(), distance_set.end());
    GradeTable<int, rivet::ExactValue> grade_table(x_coords, y_coords);

    // free the memory
    std::vector<int>().swap(x_coords);
    std::vector<rivet::ExactValue>().swap(y_coords);
    rivet::ExactSet().swap(distance_set);

    // Create a copy of the distance matrix for sorting
    std::vector<std::vector<rivet::ExactValue>> sorted_dists;
    sorted_dists.reserve(n);
    rivet::ExactValueComparator cmp;
    for (int i = 0; i < n; ++i) {
        sorted_dists.emplace_back(D[i]);
        std::sort(sorted_dists.back().begin(), sorted_dists.back().end(), cmp);
    }

    // Prepare for GGraph construction with exact sizes
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n * n + n * (n - 1) / 2);
    std::vector<GradePoint> vertex_grades;
    vertex_grades.reserve(n * n);
    std::vector<GradePoint> edge_grades;
    edge_grades.reserve(n * n + n * (n - 1) / 2);

    // Create vertex grades
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int v = i * n + j;
            int x_rank = n - 1 - j;  // negative degree to positive rank
            int y_rank = distance_to_index[sorted_dists[i][j].exact_value];
            vertex_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    // Add edges from the same row
    for (int i = 0; i < n; ++i) {
        int base = i * n;
        // Add edges from the same row e_j = (j, j+1), j = 0, 1, ..., n-2
        // f(e_j) = (-j, r_{j+1})
        for (int j = 0; j < n - 1; ++j) {
            int x_rank = n - 1 - j;
            int y_rank = distance_to_index[sorted_dists[i][j + 1].exact_value];
            edges.emplace_back(base + j, base + j + 1);
            edge_grades.emplace_back(GradePoint(x_rank, y_rank));
        }
    }

    // Add edges across vertices
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // compute max distance between i and j
            std::vector<rivet::ExactValue> max_r;
            max_r.reserve(n);
            for (int k = 0; k < n; ++k)
                max_r.emplace_back(std::max(sorted_dists[i][k], sorted_dists[j][k], cmp));

            // find the first index k such that max_r[k] >= D[i][j]
            rivet::ExactValue threshold = D[i][j];
            auto it = std::lower_bound(max_r.begin(), max_r.end(), threshold, cmp);
            int idx = it - max_r.begin();

            // add edges from i to j
            for (int k = idx; k < n; ++k) {
                // transform to 1D vertex index
                int ii = i * n + k;
                int jj = j * n + k;
                edges.emplace_back(ii, jj);
                int x_rank = n - 1 - k;
                int y_rank = distance_to_index[max_r[k].exact_value];
                edge_grades.emplace_back(GradePoint(x_rank, y_rank));
            }
        }
    }

    return std::make_tuple(GGraph(n * n, vertex_grades, edges, edge_grades),
                           std::move(grade_table));
}