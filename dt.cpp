#include "Tree.hpp"

int main() {
    std::shared_ptr<Node> root1 = nullptr;
    insert(root1, 10);
    insert(root1, 5);
    insert(root1, 15);
    insert(root1, 3);
    insert(root1, 7);
    /*  
           15
         /   \
        7     10
       / \
      3   5
    */   

    std::shared_ptr<Node> root2 = nullptr;
    insert(root2, 20);
    insert(root2, 25);
    insert(root2, 22);
    insert(root2, 30);

    std::cout << "Tree 1: ";
    printTree(root1);
    std::cout << "Tree 2: ";
    printTree(root2);

    auto path1 = getPathToRoot(root1->left); // Path to root from node 7
    while (!path1.empty()) {
        std::shared_ptr<Node> current = path1.front();
        std::cout << current->value << " ";
        path1.erase(path1.begin());
    }
    std::cout << std::endl;

    // auto path2 = getPathToRoot(root2->left); // Path to root from node 20

    // auto mergedRoot = mergePaths(root1->left, root2->left);
    // if (mergedRoot) {
    //     std::cout << "\nMerged Tree: ";
    //     printTree(mergedRoot);
    //     std::cout << "\n";
    // }

    return 0;
}
