#pragma once


#include <icecream.hpp>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>
#include <iostream>

#include "Graph.hpp"

template<typename T>
std::vector<std::vector<T>> read_points(const std::string& filename) {
    // read points from a file, each line is a point in R ^d is in the format of x1,x2,...,xd

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
template<typename T>
std::tuple<int, std::vector<std::pair<int, int>>, std::vector<std::pair<T, T>>, std::vector<std::pair<T, T>>>
read_filtration_data(const std::string& file_name) {
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
        } 
        else if (line == "Es") {
            section = "Es";
        } 
        else if (line == "F_Vs") {
            section = "F_Vs";
        } 
        else if (line == "F_Es") {
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
        }
        else if (section == "F_Vs") {
            // Remove commas and spaces
            line.erase(std::remove(line.begin(), line.end(), ','), line.end());
            std::stringstream ss(line);
            double fv1, fv2;
            ss >> fv1 >> fv2;
            F_Vs.push_back(std::make_pair(fv1, fv2));
        }
        else if (section == "F_Es") {
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



template<typename PT, typename FT> // PT: Point type, FT: Filtration value type
std::tuple<std::vector<int>, std::vector<std::pair<int, int>>, std::vector<FT>, std::vector<FT>>
point_cloud_to_1_critical_filtration(const std::vector<std::vector<PT>>& points, bool x_y_swapped = false) {
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
                filt_func_v.emplace_back(FT(y,x));
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
        const auto& di = sorted_dists[i];
        for (int j = i + 1; j < n; ++j) {
            const auto& dj = sorted_dists[j];
            std::vector<PT> max_r(n);
            for (int k = 0; k < n; ++k)
                max_r[k] = std::max(di[k], dj[k]);

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