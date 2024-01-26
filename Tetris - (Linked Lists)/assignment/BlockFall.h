#ifndef BLOCKFALL_H
#define BLOCKFALL_H

#define occupiedCellChar "██"
#define unoccupiedCellChar "▒▒"

#include <vector>
#include <string>

#include "Block.h"
#include "LeaderboardEntry.h"
#include "Leaderboard.h"

using namespace std;

class BlockFall {
public:

    BlockFall(string grid_file_name,
              string blocks_file_name,
              bool gravity_mode_on,
              const string &leaderboard_file_name,
              const string &player_name)
    ;

    virtual ~BlockFall();

    int rows;  // Number of rows in the grid
    int cols;  // Number of columns in the grid
    vector<vector<int> > grid;  // 2D game grid
    vector<vector<bool>> power_up; // 2D matrix of the power-up shape
    Block * initial_block = nullptr; // Head of the list of game blocks. Must be filled up and initialized after a call to read_blocks()
    Block * active_rotation = nullptr; // Currently active rotation of the active block. Must start with the initial_block
    bool gravity_mode_on = false; // Gravity mode of the game
    unsigned long current_score = 0; // Current score of the game
    string leaderboard_file_name; // Leaderboard file name, taken from the command-line argument 5 in main
    string player_name; // Player name, taken from the command-line argument 6 in main
    Leaderboard leaderboard;
    Block * last_block = nullptr;
    int won=0;

    vector<vector<bool>> applyRightRotation(const vector<vector<bool>>& input);
    void initialize_grid(const string & input_file); // Initializes the grid using the command-line argument 1 in main
    void read_blocks(const string & input_file); // Reads the input file and calls the read_block() function for each block;
    void add_block(const vector<vector<bool>>& shape);


    void drop();
    bool isValidMove(Block* block, int curX, int curY, int deltaX, int deltaY) const;
    void moveBlock(Block* block, int deltaX, int deltaY);

    void printGrid() const;

    void moveRight();

    void moveLeft();

    void rotateRight();

    void rotateLeft();

    void toggleGravity();

    void updateGrid(Block *block);

    bool isValidMoveGra(Block *block, int curX, int curY, int dx, int dy, int pieceX, int pieceY) const;

    int countOccupiedCells(Block *block) const;

    void checkAndClearRows();

    bool hasCollision(Block *block, int dx, int dy) const;

    bool isPartOfActiveBlock(int x, int y) const;

    bool isPowerUpActivated() const;

    bool isRotationValid(Block *oldRotation, Block *newRotation) const;

    void gameOver() const;

    void deletePowerUp();
};



#endif // BLOCKFALL_H
