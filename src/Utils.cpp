#include "Utils.hpp"

// Sort and count betti result
// e.g. betti_result = [(0, 1), (0, 2), (0, 1), (1, 1), (1, 2), (1, 1)]
// output = [(0, 1, 2), (0, 2, 1), (1, 1, 2), (1, 2, 1)]
std::vector<std::tuple<int, int, int>> sort_count_betti_result(
    std::vector<std::pair<int, int>> &betti_result) {
    std::map<std::pair<int, int>, int> count_map;

    // Step 1: Count occurrences
    for (const auto &p : betti_result) {
        count_map[p]++;
    }

    // Step 2: Convert to vector of tuples
    std::vector<std::tuple<int, int, int>> result;
    for (const auto &[key, count] : count_map) {
        result.emplace_back(key.first, key.second, count);
    }
    return result;
}
