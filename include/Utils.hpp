#pragma once
#include <map>
#include <tuple>
#include <utility>
#include <vector>

// Sort and count betti result
// e.g. betti_result = [(0, 1), (0, 2), (0, 1), (1, 1), (1, 2), (1, 1)]
// output = [(0, 1, 2), (0, 2, 1), (1, 1, 2), (1, 2, 1)]
std::vector<std::tuple<int, int, int>> sort_count_betti_result(
    std::vector<std::pair<int, int>> &betti_result);
