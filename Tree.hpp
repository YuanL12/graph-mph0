#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>
#include <string>

// Define the Node structure
struct Node {
    double value;
    //naming rule: vertex w (a single intger); edge (v,w) pair of integers
    std::string label; 
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::weak_ptr<Node> parent;

    Node(double val, std::string label_) : value(val), label(label_),
        left(nullptr), right(nullptr), parent() {}
};

// Function to swap the values of two nodes
void swapValues(std::shared_ptr<Node> node1, std::shared_ptr<Node> node2) {
    std::swap(node1->value, node2->value);
}


// // Insert a value into the heap, ensuring heap order is maintained
// std::shared_ptr<Node> insert(std::shared_ptr<Node>& root, double value) {
//     if (!root) {
//         root = std::make_shared<Node>(value, "root");
//         return root;
//     }

//     // Use a queue to perform level-order traversal to find the correct position.
//     // It means that the new node is inserted at the first available position 
//     // in a level-order manner, maintaining the complete binary tree property.
//     std::vector<std::shared_ptr<Node>> queue = {root};
//     std::shared_ptr<Node> newNode;

//     while (!queue.empty()) {
//         std::shared_ptr<Node> current = queue.front();
//         queue.erase(queue.begin());

//         if (!current->left) {
//             current->left = std::make_shared<Node>(value);
//             current->left->parent = current;
//             newNode = current->left;
//             break;
//         } else {
//             queue.push_back(current->left);
//         }

//         if (!current->right) {
//             current->right = std::make_shared<Node>(value);
//             current->right->parent = current;
//             newNode = current->right;
//             break;
//         } else {
//             queue.push_back(current->right);
//         }
//     }

//     // Heapify up to maintain the heap property
//     while (newNode->parent.lock() && newNode->value > newNode->parent.lock()->value) {
//         swapValues(newNode, newNode->parent.lock());
//         newNode = newNode->parent.lock();
//     }

//     return newNode;
// }

// Print the tree (level-order traversal)
void printTree(std::shared_ptr<Node> root) {
    if (!root) return;

    std::vector<std::shared_ptr<Node>> queue = {root};
    while (!queue.empty()) {
        std::shared_ptr<Node> current = queue.front();
        queue.erase(queue.begin());

        std::cout << current->value << " ";

        if (current->left) queue.push_back(current->left);
        if (current->right) queue.push_back(current->right);
    }
    std::cout << std::endl;
}



// Retrieve the path from a given node to the root(bottom-to-top, i.e., ascending order)
// true for left, false for right
std::vector<std::pair<std::shared_ptr<Node>, bool>> getPathToRoot(std::shared_ptr<Node> node) {
    std::vector<std::pair<std::shared_ptr<Node>, bool>> path;
    std::shared_ptr<Node> currentNode = node;
    path.push_back(std::make_pair(node, true)); // dummy direction 
    while (currentNode) {
        std::shared_ptr<Node> parent = currentNode->parent.lock(); // Get the parent
        if (parent) {
            if (parent->left == currentNode) {
                path.push_back(std::make_pair(parent, true)); // Left child
            } else if (parent->right == currentNode) {
                path.push_back(std::make_pair(parent, false)); // Right child
            }
        }
        currentNode = parent;
    }
    return path;
}

// Find the lowest common ancestor (LCA) of two nodes
std::shared_ptr<Node> findLCA(std::shared_ptr<Node> node1, std::shared_ptr<Node> node2) {
    auto path1 = getPathToRoot(node1);
    auto path2 = getPathToRoot(node2);
    std::reverse(path1.begin(), path1.end());
    std::reverse(path2.begin(), path2.end());
    std::shared_ptr<Node> lca = nullptr;
    for (size_t i = 0; i < std::min(path1.size(), path2.size()); ++i) {
        if (path1[i] == path2[i]) {
            lca = path1[i].first;
        } else {
            break;
        }
    }
    return lca;
}

// std::shared_ptr<Node> mergePaths(std::shared_ptr<Node> node1, std::shared_ptr<Node> node2) {
//     std::vector<std::shared_ptr<Node>> path1 = getPathToRoot(node1);
//     std::vector<std::shared_ptr<Node>> path2 = getPathToRoot(node2);

//     // New path vector to hold the merged path
//     std::vector<std::shared_ptr<Node>> mergedPath;

//     // Merging two paths, ensuring the heap property
//     size_t i = 0, j = 0;
//     while (i < path1.size() && j < path2.size()) {
//         if (path1[i]->value >= path2[j]->value) {
//             mergedPath.push_back(path1[i++]);
//         } else {
//             mergedPath.push_back(path2[j++]);
//         }
//     }

//     // Append the remaining nodes from path1 or path2
//     while (i < path1.size()) mergedPath.push_back(path1[i++]);
//     while (j < path2.size()) mergedPath.push_back(path2[j++]);

//     // Now we need to set the parent-child relationships correctly in the merged path
//     for (size_t k = 0; k < mergedPath.size() - 1; ++k) {
//         if (mergedPath[k]->left) mergedPath[k]->left->parent.reset();  // Reset old parent
//         if (mergedPath[k]->right) mergedPath[k]->right->parent.reset(); // Reset old parent

//         if (!mergedPath[k]->left) {
//             mergedPath[k]->left = mergedPath[k + 1];
//         } else if (!mergedPath[k]->right) {
//             mergedPath[k]->right = mergedPath[k + 1];
//         } else {
//             // Both child positions are occupied; this should not happen if the path is correct
//             // Need to handle this case appropriately, possibly restructuring the tree
//         }

//         mergedPath[k + 1]->parent = mergedPath[k];
//     }

//     // Return the root of the new merged path
//     return mergedPath.front();
// }

