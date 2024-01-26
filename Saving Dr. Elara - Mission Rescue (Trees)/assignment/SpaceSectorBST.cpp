#include <queue>
#include "SpaceSectorBST.h"

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    deleteTree(root);
}

void SpaceSectorBST::deleteTree(Sector* node) {
    if (node != nullptr) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string& filename) {
    ifstream file(filename);
    string line;
    getline(file, line);  // Skip first line

    int x, y, z;
    char delimiter;
    while (file >> x >> delimiter >> y >> delimiter >> z) {
        insertSectorByCoordinates(x, y, z);
    }
    file.close();
}


void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    root = insert(root, x, y, z);
}

Sector* SpaceSectorBST::insert(Sector* node, int x, int y, int z) {
    if (node == nullptr) {
        Sector* newNode = new Sector(x, y, z);
        newNode->parent = nullptr;
        return newNode;
    }

    // Inserting in the left subtree
    if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) {
        if (node->left == nullptr) {
            node->left = new Sector(x, y, z);
            node->left->parent = node;
        } else {
            insert(node->left, x, y, z);
        }
    }

        // Inserting in the right subtree
    else {
        if (node->right == nullptr) {
            node->right = new Sector(x, y, z);
            node->right->parent = node;
        } else {
            insert(node->right, x, y, z);
        }
    }

    return node;
}


void SpaceSectorBST::deleteSector(const std::string& sector_code) {
    if (root == nullptr) {
        return;
    }

    std::queue<Sector*> q;
    q.push(root);

    Sector* target = nullptr;
    Sector* parent = nullptr;

    while (!q.empty()) {
        Sector* current = q.front();
        q.pop();

        if (current->sector_code == sector_code) {
            target = current;
            break;
        }
        parent = current;

        if (current->left) {
            if (current->left->sector_code == sector_code) {
                target = current->left;
                break;
            }
            q.push(current->left);
        }
        if (current->right) {
            if (current->right->sector_code == sector_code) {
                target = current->right;
                break;
            }
            q.push(current->right);
        }
    }

    if (!target) {
        return;
    }

    // Replace target with in-order successor if it has two children
    if (target->left && target->right) {
        Sector* successorParent = target;
        Sector* successor = target->right;
        while (successor->left) {
            successorParent = successor;
            successor = successor->left;
        }
        target->sector_code = successor->sector_code;
        target = successor;  // Update target to be the successor
        parent = successorParent;  // Update parent to be the successor's parent
    }

    // If target has only one child or no child
    Sector* child = (target->left) ? target->left : target->right;
    if (parent == nullptr) {
        root = child;
    } else if (parent->left == target) {
        parent->left = child;
    } else {
        parent->right = child;
    }
    delete target;
}


void SpaceSectorBST::displaySectorsInOrder() {
    std::cout << "Space sectors inorder traversal:" << std::endl;
    inOrderTraversal(root);
    std::cout << std::endl;  // Newline at the end
}

void SpaceSectorBST::inOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    inOrderTraversal(node->left);
    std::cout << node->sector_code << std::endl;
    inOrderTraversal(node->right);
}


void SpaceSectorBST::displaySectorsPreOrder() {
    std::cout << "Space sectors preorder traversal:" << std::endl;
    preOrderTraversal(root);
    std::cout << std::endl;  // Newline at the end
}

void SpaceSectorBST::preOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    std::cout << node->sector_code << std::endl;
    preOrderTraversal(node->left);
    preOrderTraversal(node->right);
}


void SpaceSectorBST::displaySectorsPostOrder() {
    std::cout << "Space sectors postorder traversal:" << std::endl;
    postOrderTraversal(root);
    std::cout << std::endl;  // Newline at the end
}

void SpaceSectorBST::postOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    postOrderTraversal(node->left);
    postOrderTraversal(node->right);
    std::cout << node->sector_code << std::endl;
}


std::vector<Sector*> SpaceSectorBST::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> path;
    findPath(root, sector_code, path);
    return path;
}



void SpaceSectorBST::findPath(Sector* node, const std::string& sector_code, std::vector<Sector*>& path) {
    if (node == nullptr) {
        return;
    }

    path.push_back(node);

    // Check if the current node's sector code matches the target sector code
    if (node->sector_code == sector_code) {
        return;
    }

    // Recursively search in the left subtree
    findPath(node->left, sector_code, path);
    if (!path.empty() && path.back()->sector_code == sector_code) {
        return;
    }

    // Recursively search in the right subtree
    findPath(node->right, sector_code, path);
    if (!path.empty() && path.back()->sector_code == sector_code) {
        return;
    }

    // If the target sector is not found in either subtree, remove the current node from the path
    path.pop_back();
}


void SpaceSectorBST::printStellarPath(const std::vector<Sector*>& path) {
    if (path.empty()){
        std::cout << "A path to Dr. Elara could not be found." << std::endl;
        return;
    }
    std::cout << "The stellar path to Dr. Elara: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i]->sector_code;
        if (i < path.size() - 1) std::cout << "->";
    }
    std::cout << std::endl;
}
