#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>  // for std::ifstream and std::ofstream
#include <icecream.hpp>
#include <iostream>
#include <set>      // for std::set
#include <sstream>  // for std::stringstream
#include <string>   // for std::string
#include <utility>
#include <vector>

#include "GGraph.hpp"  // Add this for GGraph class
#include "Grade.hpp"   // Must include before using GradeTableVariant
#include "Graph.hpp"
#include "PointCloud.hpp"
#include "Timer.hpp"
#include "Utils.hpp"

inline void print_grade_table(const GradeTableVariant &grade_table, bool x_y_swap = false) {
    std::visit([&](auto &&tbl) { tbl.print(x_y_swap); }, grade_table);
}

inline void print_grade_table_size(const GradeTableVariant &grade_table, bool x_y_swap = false) {
    std::visit(
        [&](auto &&tbl) {
            std::cout << "GradeTable(" << tbl.get_x_size() << ", " << tbl.get_y_size() << ")"
                      << std::endl;
        },
        grade_table);
}

inline void write_betti_numbers(const std::vector<std::tuple<int, int, int>> &betti_0,
                                const std::vector<std::tuple<int, int, int>> &betti_1,
                                const std::vector<std::tuple<int, int, int>> &betti_2,
                                const std::vector<std::tuple<int, int, int>> &betti_0_1,
                                const std::string &filename) {
    // write the betti numbers to a file
    std::ofstream file(filename);
    file << "betti_0:" << std::endl;
    for (const auto &b0 : betti_0) {
        file << "(" << std::get<0>(b0) << ", " << std::get<1>(b0) << ", " << std::get<2>(b0) << ")"
             << std::endl;
    }
    file << "betti_1:" << std::endl;
    for (const auto &b1 : betti_1) {
        file << "(" << std::get<0>(b1) << ", " << std::get<1>(b1) << ", " << std::get<2>(b1) << ")"
             << std::endl;
    }
    file << "betti_2:" << std::endl;
    for (const auto &b2 : betti_2) {
        file << "(" << std::get<0>(b2) << ", " << std::get<1>(b2) << ", " << std::get<2>(b2) << ")"
             << std::endl;
    }
    file << "betti_0_1:" << std::endl;
    for (const auto &b01 : betti_0_1) {
        file << "(" << std::get<0>(b01) << ", " << std::get<1>(b01) << ", " << std::get<2>(b01)
             << ")" << std::endl;
    }
}

inline void print_and_write_betti_result(std::vector<std::pair<int, int>> &raw_betti_0,
                                         std::vector<std::pair<int, int>> &raw_betti_1,
                                         std::vector<std::pair<int, int>> &raw_betti_2,
                                         std::vector<std::pair<int, int>> &raw_betti_0_1,
                                         bool x_y_swap = false, std::string file_name = "") {
    if (x_y_swap) {
        std::cout << "Swap x, y axis in print_and_write_betti_result" << std::endl;
        for (auto &b0 : raw_betti_0) {
            std::swap(b0.first, b0.second);
        }
        for (auto &b1 : raw_betti_1) {
            std::swap(b1.first, b1.second);
        }
        for (auto &b2 : raw_betti_2) {
            std::swap(b2.first, b2.second);
        }
        for (auto &b01 : raw_betti_0_1) {
            std::swap(b01.first, b01.second);
        }
    }
    auto betti_0 = sort_count_betti_result(raw_betti_0);
    auto betti_1 = sort_count_betti_result(raw_betti_1);
    auto betti_2 = sort_count_betti_result(raw_betti_2);
    auto betti_0_1 = sort_count_betti_result(raw_betti_0_1);
    IC(betti_0.size(), betti_1.size(), betti_2.size(), betti_0_1.size());

    if (file_name != "") {
        // write the betti numbers to a file
        write_betti_numbers(betti_0, betti_1, betti_2, betti_0_1, file_name);
    }
}

template <typename T>
inline std::vector<std::vector<T>> read_points(const std::string &filename) {
    // read points from a file, each line is a point in R ^d is in the format of
    // x1,x2,...,xd

    // raise an runtime error if the file is not found
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("File not found: " + filename);
    }

    // read each line to load into points
    std::vector<std::vector<T>> points;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<T> point;
        std::stringstream ss(line);
        std::string value;

        // Read comma-separated values
        while (std::getline(ss, value, ',')) {
            std::stringstream value_stream(value);
            T x;
            if (value_stream >> x) {
                point.push_back(x);
            }
        }
        points.push_back(point);
    }
    return points;
}

// read filtration data from a file
template <typename T>
inline std::tuple<int, std::vector<std::pair<int, int>>, std::vector<std::pair<T, T>>,
                  std::vector<std::pair<T, T>>>
read_filtration_data(const std::string &file_name) {
    // number of vertices
    int nV;
    // edges
    std::vector<std::pair<int, int>> Es;
    // filtration values of vertices
    std::vector<std::pair<double, double>> F_Vs;
    // filtration values of edges
    std::vector<std::pair<double, double>> F_Es;
    // start reading the file
    // if the file is not found, throw an runtime error
    std::ifstream file(file_name);
    if (!file.is_open()) {
        throw std::runtime_error("File not found: " + file_name);
    }

    std::string line;
    std::string section;

    // Read the file line by line
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Check for section headers
        if (line == "nV") {
            section = "nV";
            std::getline(file, line);
            nV = std::stoi(line);
        } else if (line == "Es") {
            section = "Es";
        } else if (line == "F_Vs") {
            section = "F_Vs";
        } else if (line == "F_Es") {
            section = "F_Es";
        }
        // Process data based on current section
        else if (section == "Es") {
            // Remove commas and spaces
            line.erase(std::remove(line.begin(), line.end(), ','), line.end());
            std::stringstream ss(line);
            int v1, v2;
            ss >> v1 >> v2;
            Es.push_back(std::make_pair(v1, v2));
        } else if (section == "F_Vs") {
            // Remove commas and spaces
            line.erase(std::remove(line.begin(), line.end(), ','), line.end());
            std::stringstream ss(line);
            double fv1, fv2;
            ss >> fv1 >> fv2;
            F_Vs.push_back(std::make_pair(fv1, fv2));
        } else if (section == "F_Es") {
            // Remove commas and spaces
            line.erase(std::remove(line.begin(), line.end(), ','), line.end());
            std::stringstream ss(line);
            double fe1, fe2;
            ss >> fe1 >> fe2;
            F_Es.push_back(std::make_pair(fe1, fe2));
        }
    }
    return std::make_tuple(nV, Es, F_Vs, F_Es);
}

template <typename PT, typename FT>  // PT: Point type, FT: Filtration value type
inline std::tuple<std::vector<int>, std::vector<std::pair<int, int>>, std::vector<FT>,
                  std::vector<FT>>
point_cloud_to_1_critical_filtration(const std::vector<std::vector<PT>> &points,
                                     bool x_y_swapped = false) {
    /*
    Convert a point cloud to a 1-critical filtration. The added vertex index follows
    the row-major order of the distance matrix.
    ------------------------------------------------------------
    Args:
        points: list of points in R^d
        x_y_swapped: if true, the x and y coordinates are swapped
    Returns:
        vertices: list of vertices
        edges: list of edges
        filt_func_v: list of vertices in the filtration function
        filt_func_e: list of edges in the filtration function
    */
    const int n = points.size();
    std::vector<int> vertices(n * n);
    std::vector<int> degrees(n);
    std::vector<std::vector<PT>> D(n, std::vector<PT>(n));

    // Compute distance matrix
    for (int i = 0; i < n; ++i) {
        degrees[i] = i;
        for (int j = 0; j < n; ++j) {
            PT dist = 0.0;
            for (size_t k = 0; k < points[i].size(); ++k)
                dist += (points[i][k] - points[j][k]) * (points[i][k] - points[j][k]);
            D[i][j] = std::sqrt(dist);
        }
    }

    // Precompute sorted distances
    std::vector<std::vector<PT>> sorted_dists(n, std::vector<PT>(n));
    for (int i = 0; i < n; ++i) {
        sorted_dists[i] = D[i];
        std::sort(sorted_dists[i].begin(), sorted_dists[i].end());
    }

    // Create vertices
    std::vector<FT> filt_func_v;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            vertices[i * n + j] = i * n + j;
            PT x = sorted_dists[i][j];
            PT y = -degrees[j];
            if (x_y_swapped)
                filt_func_v.emplace_back(FT(y, x));
            else
                filt_func_v.emplace_back(FT(x, y));
        }
    }

    // Horizontal edges in each row
    std::vector<std::pair<int, int>> edges;
    std::vector<FT> filt_func_e;
    for (int i = 0; i < n; ++i) {
        int base = i * n;
        for (int j = 0; j < n - 1; ++j) {
            edges.emplace_back(base + j, base + j + 1);
            PT x = sorted_dists[i][j + 1];
            PT y = -degrees[j];
            if (x_y_swapped)
                filt_func_e.emplace_back(FT(y, x));
            else
                filt_func_e.emplace_back(FT(x, y));
        }
    }

    // Pairwise edges across vertices
    for (int i = 0; i < n; ++i) {
        const auto &di = sorted_dists[i];
        for (int j = i + 1; j < n; ++j) {
            const auto &dj = sorted_dists[j];
            std::vector<PT> max_r(n);
            for (int k = 0; k < n; ++k) max_r[k] = std::max(di[k], dj[k]);

            PT threshold = D[i][j];
            auto it = std::lower_bound(max_r.begin(), max_r.end(), threshold);
            int idx = it - max_r.begin();
            for (int k = idx; k < n; ++k) {
                // transform to 1D vertex index
                int ii = i * n + k;
                int jj = j * n + k;
                edges.emplace_back(ii, jj);
                PT x = max_r[k];
                PT y = -degrees[k];
                if (x_y_swapped)
                    filt_func_e.emplace_back(FT(y, x));
                else
                    filt_func_e.emplace_back(FT(x, y));
            }
        }
    }

    return {vertices, edges, filt_func_v, filt_func_e};
}

inline void write_filtration_data_to_firep_without_header(const GGraph &G, std::ofstream &file) {
    const auto &gedges = G.get_gedges();
    const auto &gvertices = G.get_gvertices();
    int nV = gvertices.size();
    int nE = gedges.size();

    // give each vertex a unique index starting from 0
    std::unordered_map<VertexId, size_t> vertex_2_idx;
    std::unordered_map<size_t, VertexId> idx_2_vertex;
    size_t idx = 0;
    for (const auto &gv : gvertices) {
        assert(gv.get_id() != -1 &&
               "Vertex id is -1 (removed), which is not allowed when writing out");
        vertex_2_idx[gv.get_id()] = idx;
        idx_2_vertex[idx] = gv.get_id();
        idx++;
    }

    file << nE << " " << nV << " " << 0 << std::endl;
    for (const auto &ge : gedges) {
        VertexId v0 = ge.get_v0();
        size_t v0_idx = vertex_2_idx[v0];
        VertexId v1 = ge.get_v1();
        size_t v1_idx = vertex_2_idx[v1];
        // get ranks
        int x = ge.get_grade().get_x();
        int y = ge.get_grade().get_y();
        file << x << " " << y << " ; " << v0_idx << " " << v1_idx << " " << std::endl;
    }
    for (size_t i = 0; i < nV; ++i) {
        VertexId v = idx_2_vertex[i];
        int x = gvertices[v].get_grade().get_x();
        int y = gvertices[v].get_grade().get_y();
        file << x << " " << y << " ; " << std::endl;
    }

    file << std::endl;
}

/*
Write the filtration data to scc2020 format, the input GradeGraph has to be a 1-critical
filtration The format is: scc2020
2
nE nV 0
...
*/
inline void write_filtration_data_to_scc2020(const GGraph &G, const std::string &filename) {
    // check if file exists, if not exist, create it
    std::ofstream file(filename);
    file << "scc2020" << std::endl;
    file << "2" << std::endl;  // 2-parameter filtration
    write_filtration_data_to_firep_without_header(G, file);
    file.close();
}

/*
write the filtration data to scc2020 format, the input GradeGraph has to be a 1-critical
filtration The format is:
--datatype firep
--xlabel parameter 1
--ylabel parameter 2

# data
nE nV 0
...
*/
inline void write_filtration_data_to_rivet_firep(const GGraph &G, const std::string &filename) {
    // check if file exists, if not exist, create it
    std::ofstream file(filename);
    file << "--datatype firep" << std::endl;
    file << "--xlabel parameter 1" << std::endl;
    file << "--ylabel parameter 2" << std::endl;
    file << std::endl;
    file << "# data" << std::endl;
    write_filtration_data_to_firep_without_header(G, file);
    file.close();
}

/*
Write the filtration data to mpfree format, the input GradeGraph has to be a 1-critical
filtration The format is: firep first parameter second parameter nE nV 0
...
*/
inline void write_filtration_data_to_mpfree_firep(const GGraph &G, const std::string &filename) {
    // check if file exists, if not exist, create it
    std::ofstream file(filename);
    file << "firep" << std::endl;
    file << "first parameter" << std::endl;
    file << "second parameter" << std::endl;
    write_filtration_data_to_firep_without_header(G, file);
    file.close();
}

inline std::tuple<GGraph, GradeTable<int, int>> read_filtration_data_from_firep(
    const std::string &filename) {
    // Initialize
    std::vector<int> x_coords;
    std::vector<int> y_coords;
    std::vector<GradePoint> vertex_grades;
    std::vector<std::pair<int, int>> edges;
    std::vector<GradePoint> edge_grades;
    int nE, nV;

#if MPH0_TIMERS
    mph0::load_input_timer.resume();
#endif
    // Read the filtration data from a file, then build a GGraph.
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("File not found: " + filename);
    }
    // if not end in .firep, throw an runtime error
    if (filename.find(".firep") == std::string::npos) {
        throw std::runtime_error("File is not a firep file: " + filename);
    }

    // skip the first three lines
    std::string line;
    std::getline(file, line);  // line 1: "firep"
    std::getline(file, line);  // line 2: "first parameter"
    std::getline(file, line);  // line 3: "second parameter"
    std::getline(file, line);  // line 4: "#_of_edges #_of_vertices 0"
    std::stringstream ss(line);
    int int_zero = 0;
    ss >> nE >> nV >> int_zero;

    // Validate parsed values
    if (ss.fail() || nE < 0 || nV < 0) {
        throw std::runtime_error(
            "Invalid format in line 4: expected 'nE nV 0' where nE and nV are "
            "non-negative integers");
    }

    // Check for reasonable limits to prevent memory issues
    if (nE > 10000000 || nV > 10000000) {
        std::cerr << "WARNING: File contains large number of edges or vertices (nE=" << nE
                  << ", nV=" << nV << "). This may cause memory issues." << std::endl;
    }

    edges.reserve(nE);
    vertex_grades.reserve(nV);
    edge_grades.reserve(nE);

    // first read the graded edges: format is grade_x grade_y ; boundary_vertex_0
    // boundary_vertex_1
    for (int i = 0; i < nE; ++i) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Unexpected end of file while reading edge " +
                                     std::to_string(i));
        }
        std::stringstream ss(line);
        int x, y, v0, v1;
        char semicolon;
        ss >> x >> y >> semicolon >> v0 >> v1;

        if (ss.fail() || semicolon != ';') {
            throw std::runtime_error("Invalid format in edge line " + std::to_string(i + 4) + ": " +
                                     line);
        }

        if (v0 < 0 || v1 < 0 || v0 >= nV || v1 >= nV) {
            throw std::runtime_error("Invalid vertex indices in edge line " +
                                     std::to_string(i + 4) + ": v0=" + std::to_string(v0) +
                                     ", v1=" + std::to_string(v1) + " (nV=" + std::to_string(nV) +
                                     ")");
        }

        edges.emplace_back(v0, v1);
        edge_grades.emplace_back(x, y);
        x_coords.push_back(x);
        y_coords.push_back(y);
    }

    // then read the graded vertices
    for (int i = 0; i < nV; ++i) {
        if (!std::getline(file, line)) {
            throw std::runtime_error("Unexpected end of file while reading vertex " +
                                     std::to_string(i));
        }
        std::stringstream ss(line);
        int x, y;
        char semicolon;
        ss >> x >> y >> semicolon;

        if (ss.fail() || semicolon != ';') {
            throw std::runtime_error("Invalid format in vertex line " + std::to_string(i + 4 + nE) +
                                     ": " + line);
        }

        vertex_grades.emplace_back(x, y);
        x_coords.push_back(x);
        y_coords.push_back(y);
    }

#if MPH0_TIMERS
    mph0::load_input_timer.stop();
#endif

    // Remove the duplicated x and y coordinates and sort them
    std::sort(x_coords.begin(), x_coords.end());
    x_coords.erase(std::unique(x_coords.begin(), x_coords.end()), x_coords.end());
    std::sort(y_coords.begin(), y_coords.end());
    y_coords.erase(std::unique(y_coords.begin(), y_coords.end()), y_coords.end());

    // Give each x and y coordinate a unique index/rank
    std::unordered_map<int, int> x_rank_map;
    std::unordered_map<int, int> y_rank_map;
    for (int i = 0; i < x_coords.size(); ++i) {
        x_rank_map[x_coords[i]] = i;
    }
    for (int i = 0; i < y_coords.size(); ++i) {
        y_rank_map[y_coords[i]] = i;
    }

    // Construct a GradeTable with x and y coordinates
    GradeTable<int, int> grade_table(x_coords, y_coords);

    // free the memory
    std::vector<int>().swap(x_coords);
    std::vector<int>().swap(y_coords);

    // Convert the grades to ranks
    for (int i = 0; i < vertex_grades.size(); ++i) {
        vertex_grades[i].x = x_rank_map[vertex_grades[i].x];
        vertex_grades[i].y = y_rank_map[vertex_grades[i].y];
    }
    for (int i = 0; i < edge_grades.size(); ++i) {
        edge_grades[i].x = x_rank_map[edge_grades[i].x];
        edge_grades[i].y = y_rank_map[edge_grades[i].y];
    }

    // Create a GGraph
    GGraph ggraph(nV, vertex_grades, edges, edge_grades);

    return std::make_tuple(std::move(ggraph), std::move(grade_table));
}

template <typename PT>
std::tuple<GGraph, GradeTable<PT, int>> build_degree_filtration_from_point_cloud(
    const std::string &filename) {
#if MPH0_TIMERS
    mph0::load_input_timer.resume();
#endif

    // read the points
    auto points = read_points<PT>(filename);

#if MPH0_TIMERS
    mph0::load_input_timer.stop();
#endif

    // build the degree filtration
    return point_cloud_to_degree_Rips_filtration<PT>(points);
}

template <typename PT>
std::tuple<GGraph, GradeTable<double, PT>> build_ball_density_filtration_from_point_cloud(
    const std::string &filename) {
#if MPH0_TIMERS
    mph0::load_input_timer.resume();
#endif

    // read the points
    auto points = read_points<PT>(filename);

#if MPH0_TIMERS
    mph0::load_input_timer.stop();
#endif

    // build the ball density filtration
    return point_cloud_to_ball_density_Rips_filtration<PT>(points);
}

template <typename PT>
std::tuple<GGraph, GradeTable<int, rivet::ExactValue>>
build_degree_filtration_from_point_cloud_rational(const std::string &filename) {
    // read the points
    auto points = read_points<PT>(filename);
    return point_cloud_to_degree_Rips_filtration_rational<PT>(points);
}

template <typename PT>
std::tuple<GGraph, GradeTable<rivet::ExactValue, rivet::ExactValue>>
build_ball_density_filtration_from_point_cloud_rational(const std::string &filename) {
    // read the points
    auto points = read_points<PT>(filename);
    return point_cloud_to_ball_density_Rips_filtration_rational<PT>(points);
}
