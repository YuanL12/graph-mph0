#include "Dendrogram.hpp"
#include <iostream>
#include <vector>
#include <cassert>

int main() {
    // Define a set of vertices
    std::vector<int> vertices = {1, 2};

    // Create a Dendrogram object with the given vertices
    Dendrogram<double> dendrogram(vertices);

    // Merge nodes (1, 2) at time 2.0
    dendrogram.merge_at_time(1, 2, 0, 3.0);

    // Merge nodes (1, 2) at time 3.0
    dendrogram.merge_at_time(1, 2, 1, 1.0);

    // Merge nodes (1, 2) at time 5.0
    dendrogram.merge_at_time(1, 2, 2, 5.0);

    // Check the structure of the dendrogram
    // In a complete test, you should verify the actual structure of the tree.
    // Here, we just print the results for manual inspection.

    // Get the path to the root for a leaf node
    auto path = getPathToRoot(dendrogram.get_vertex_node(1));
    
    std::cout << "Path to root for vertex 5:\n";
    for (const auto& [node, isLeft] : path) {
        std::cout << "Node Value: " << node->value << ", Is Left: " << isLeft << std::endl;
    }

    // Get the time of merge for two vertices
    double merge_time = dendrogram.time_of_merge_double(1, 2);
    std::cout << "Time of merge for nodes 1 and 2: " << merge_time << std::endl;

    // Add assertions or checks as needed for automated testing
    assert(merge_time == 1.0); // Expected time of merge for nodes 1 and 3

    // You can add more assertions to verify the state of the dendrogram
    // after each operation.

    return 0;
}
