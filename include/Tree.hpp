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


// Overload the << operator outside the class
std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << "Node(value: " << node.value << ", label: " << node.label << ")";
    return os;
}



// Function to swap the values of two nodes
void swapValues(std::shared_ptr<Node> node1, std::shared_ptr<Node> node2) {
    std::swap(node1->value, node2->value);
}

// Print the tree (level-order traversal)
void printTree(const std::shared_ptr<Node>& node, int indent = 0) {
    if (node) {
        if (node->right) {
            printTree(node->right, indent + 4);
        }
        if (indent) {
            std::cout << std::string(indent, ' ');
        }
        if (node->right) std::cout << " /\n" << std::string(indent, ' ');
        std::cout << node->label << "\n ";
        if (node->left) {
            std::cout << std::string(indent, ' ') << " \\\n";
            printTree(node->left, indent + 4);
        }
    }
}

void printBT(const std::string& prefix, const std::shared_ptr<Node>& node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        std::cout << node->label << std::endl;

        // enter the next tree level - left and right branch
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void printBT(const std::shared_ptr<Node>& node)
{
    printBT("", node, false);    
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



// Retrieve the path from a given node to the root(bottom-to-top, i.e., ascending order)
// true for left, false for right
std::shared_ptr<Node> getRoot(std::shared_ptr<Node> node) {
    if (!node) {
        return nullptr;  // If the node is null, return null
    }
    std::shared_ptr<Node> currentNode = node;
    while (currentNode->parent.lock()) {
        currentNode = currentNode->parent.lock();
    }
    return currentNode;
}



// Find the lowest common ancestor (LCA) of two nodes
std::shared_ptr<Node> findLCA(std::shared_ptr<Node> node1, std::shared_ptr<Node> node2) {
    auto path1 = getPathToRoot(node1);
    auto path2 = getPathToRoot(node2);
    std::reverse(path1.begin(), path1.end());
    std::reverse(path2.begin(), path2.end());
    std::shared_ptr<Node> lca = nullptr;
    for (size_t i = 0; i < std::min(path1.size(), path2.size()); ++i) {
        if (path1[i].first == path2[i].first) {
            lca = path1[i].first;
        } else {
            break;
        }
    }
    return lca;
}