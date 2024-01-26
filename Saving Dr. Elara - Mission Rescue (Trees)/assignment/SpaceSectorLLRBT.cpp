#include <stack>
#include <unordered_set>
#include "SpaceSectorLLRBT.h"

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string& filename) {
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

SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    deleteTree(root);
}

void SpaceSectorLLRBT::deleteTree(Sector* node) {
    if (node != nullptr) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}


void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    root = insert(root, nullptr, x, y, z);
    if (root) {
        root->color = BLACK;
    }
}


Sector* SpaceSectorLLRBT::insert(Sector* node, Sector* parent, int x, int y, int z) {
    if (node == nullptr) {
        Sector* newNode = new Sector(x, y, z);  // New nodes are red by default
        if (x==0 && y==0 && z==0) earth = newNode;
        newNode->parent = parent;  // Set parent pointer
        return newNode;
    }

    if (makeComparison(x, y, z, node)) {
        node->left = insert(node->left, node, x, y, z);
    } else {
        node->right = insert(node->right, node, x, y, z);
    }

    // Fixing after insertion
    if (isRed(node->right) && !isRed(node->left)) {
        node = rotateLeft(node);
    }
    if (isRed(node->left) && isRed(node->left->left)) {
        node = rotateRight(node);
    }
    if (isRed(node->left) && isRed(node->right)) {
        flipColors(node);
    }

    return node;
}

bool SpaceSectorLLRBT::makeComparison(int x, int y, int z, Sector* node) {
    return x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z);
}

// Rotate left operation
Sector* SpaceSectorLLRBT::rotateLeft(Sector* node) {
    Sector* x = node->right;
    node->right = x->left;
    if (x->left) x->left->parent = node;
    x->parent = node->parent;
    node->parent = x;
    x->left = node;
    x->color = node->color;
    node->color = RED;
    return x;
}

// Rotate right operation
Sector* SpaceSectorLLRBT::rotateRight(Sector* node) {
    Sector* x = node->left;
    node->left = x->right;
    if (x->right) x->right->parent = node;
    x->parent = node->parent;
    node->parent = x;
    x->right = node;
    x->color = node->color;
    node->color = RED;
    return x;
}

// Flip colors of a node and its children
void SpaceSectorLLRBT::flipColors(Sector* node) {
    node->color = RED;
    node->left->color = BLACK;
    node->right->color = BLACK;
}

// Check if a node is red
bool SpaceSectorLLRBT::isRed(Sector* node) {
    return node != nullptr && node->color;
}



void SpaceSectorLLRBT::displaySectorsInOrder() {
    std::cout << "Space sectors inorder traversal:" << std::endl;
    inOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::inOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    inOrderTraversal(node->left);
    std::string color_str = node->color ? "RED" : "BLACK";
    std::cout << color_str << " sector: " << node->sector_code << std::endl;
    inOrderTraversal(node->right);
}


void SpaceSectorLLRBT::displaySectorsPreOrder() {
    std::cout << "Space sectors preorder traversal:" << std::endl;
    preOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::preOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    std::string color_str = node->color ? "RED" : "BLACK";
    std::cout << color_str << " sector: " << node->sector_code << std::endl;
    preOrderTraversal(node->left);
    preOrderTraversal(node->right);
}


void SpaceSectorLLRBT::displaySectorsPostOrder() {
    std::cout << "Space sectors postorder traversal:" << std::endl;
    postOrderTraversal(root);
    std::cout << std::endl;
}

void SpaceSectorLLRBT::postOrderTraversal(Sector* node) {

    if (node == nullptr) return;

    postOrderTraversal(node->left);
    postOrderTraversal(node->right);
    std::string color_str = node->color ? "RED" : "BLACK";
    std::cout << color_str << " sector: " << node->sector_code << std::endl;
}


std::vector<Sector*> SpaceSectorLLRBT::getStellarPath(const std::string& sector_code) {
    std::vector<Sector*> pathToEarth;
    std::vector<Sector*> pathToTarget;
    std::vector<Sector*> fullPath;
    std::vector<Sector*> path;

    target = nullptr;
    find(root, sector_code, path); // this is for target, not path

    if (earth == nullptr || target == nullptr) {
        return fullPath;
    }

    getPath(root, pathToEarth, earth->sector_code);
    getPath(root, pathToTarget, sector_code);

    int intersection = -1;
    int i = 0;
    int j = 0;

    while (i < pathToEarth.size() || j < pathToTarget.size()) {
        if (i < pathToEarth.size() && j < pathToTarget.size() && pathToEarth[i] == pathToTarget[j]) {
            intersection = j;
            i++;
            j++;
        }
        else {
            break;
        }
    }

    for (int a = pathToEarth.size() - 1; a > intersection; a--)
        fullPath.push_back(pathToEarth[a]);

    for (int b = intersection; b < pathToTarget.size(); b++)
        fullPath.push_back(pathToTarget[b]);

    return fullPath;
}

bool SpaceSectorLLRBT::getPath(Sector* node, std::vector<Sector*>& path, const std::string& sector_code) {
    if (node == nullptr)
        return false;

    path.push_back(node);

    if (node->sector_code == sector_code) return true;

    if (getPath(node->left, path, sector_code) || getPath(node->right, path, sector_code)) {
        return true;
    }
    path.pop_back();
    return false;
}


void SpaceSectorLLRBT::find(Sector* node, const std::string& sector_code, std::vector<Sector*>& path) {

    if (node == nullptr) {
        return;
    }

    path.push_back(node);

    if (node->sector_code == sector_code) {
        target = node;
    }

    // Recursively search in the left subtree
    find(node->left, sector_code, path);
    if (!path.empty() && path.back()->sector_code == sector_code) {
        return;
    }

    // Recursively search in the right subtree
    find(node->right, sector_code, path);
    if (!path.empty() && path.back()->sector_code == sector_code) {
        return;
    }

    // If the target sector is not found in either subtree, remove the current node from the path
    path.pop_back();
}



void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector*>& path) {
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