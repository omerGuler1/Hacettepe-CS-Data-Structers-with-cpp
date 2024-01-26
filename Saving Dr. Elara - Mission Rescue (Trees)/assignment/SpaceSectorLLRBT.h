#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <vector>

class SpaceSectorLLRBT {
public:
    Sector* root;
    Sector* earth;
    Sector* target;
    SpaceSectorLLRBT();
    ~SpaceSectorLLRBT();
    void readSectorsFromFile(const std::string& filename);
    void insertSectorByCoordinates(int x, int y, int z);
    void displaySectorsInOrder();
    void displaySectorsPreOrder();
    void displaySectorsPostOrder();
    std::vector<Sector*> getStellarPath(const std::string& sector_code);
    void printStellarPath(const std::vector<Sector*>& path);

    static void flipColors(Sector* node);
    static bool isRed(Sector* node);
    static Sector* rotateRight(Sector* node);
    static Sector* rotateLeft(Sector* node);
    static bool makeComparison(int x, int y, int z, Sector* node);
    Sector* insert(Sector* node, Sector* parent, int x, int y, int z);
    void deleteTree(Sector* node);
    void preOrderTraversal(Sector* node);
    void postOrderTraversal(Sector* node);
    void inOrderTraversal(Sector* node);
    bool getPath(Sector* node, std::vector<Sector*>& path, const std::string& sector_code);
    void find(Sector* node, const std::string& sector_code, std::vector<Sector*>& path);
    };

#endif // SPACESECTORLLRBT_H
