#include "BlockFall.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

BlockFall::BlockFall(string grid_file_name, string blocks_file_name, bool gravity_mode_on, const string &leaderboard_file_name, const string &player_name) :
        gravity_mode_on(gravity_mode_on),
        leaderboard_file_name(leaderboard_file_name),
        player_name(player_name) {
    initialize_grid(grid_file_name);
    read_blocks(blocks_file_name);
    leaderboard.read_from_file(leaderboard_file_name);
}

void BlockFall::read_blocks(const string &input_file) {
    ifstream file(input_file);

    string line;
    int num = 0;

    vector<vector<bool>> current_block;
    while (getline(file, line)) {

        if (num%2 == 0){
            if(line[0] != '[') continue;
        }

        if (line.empty()) {
            continue;
        }

        vector<bool> row;
        stringstream ss(line);
        char c;
        while (ss >> c) {
            if (c == '['){
                num++;
                continue;
            }
            if (c == '1' || c == '0') {
                row.push_back(c - '0'); // Convert char to int
            } else if (c == '\n' || c == ' ' || c == '\t') {
                continue; // Skip whitespace and newline characters
            } else if (c == ']') {
                num++;
                break;
            }
        }
        if (c == ']') {
            current_block.push_back(row);
            add_block(current_block);
            power_up = current_block;
            current_block.clear();
            continue;
        }

        if (!row.empty()) {
            current_block.push_back(row);
        }
    }

    Block* lastt_block = initial_block;
    Block* prev_block = nullptr;

    while (lastt_block->next_block != nullptr) {
        prev_block = lastt_block;
        lastt_block = lastt_block->next_block;
    }
    if (lastt_block->sizee == 1){
        delete lastt_block;
        lastt_block = nullptr;
    } else if (lastt_block->sizee == 2){
        delete lastt_block->right_rotation;
        delete lastt_block;
    } else {
        delete lastt_block->right_rotation->right_rotation;
        delete lastt_block->right_rotation;
        delete lastt_block->left_rotation;
        delete lastt_block;
    }

    prev_block->next_block = nullptr;

    file.close();
}


void BlockFall::add_block(const vector<vector<bool>> &shape) {
    // Create a new block with the given shape
    Block *new_block = new Block;
    new_block->shape = shape;

    vector<vector<bool>> a = applyRightRotation(shape);

    vector<vector<bool>> b = applyRightRotation(a);

    vector<vector<bool>> c = applyRightRotation(b);

    if (shape == a){
        new_block->right_rotation = new_block;
        new_block->left_rotation = new_block;
        new_block->sizee = 1;
    } else if (shape == b){
        Block *new_block1 = new Block;
        new_block1->shape = applyRightRotation(shape);

        new_block->right_rotation = new_block1;
        new_block->left_rotation = new_block1;

        new_block1->right_rotation = new_block;
        new_block1->left_rotation = new_block;

        new_block->sizee = 2;
        new_block1->sizee = 2;

    } else{
        Block *new_block1 = new Block;
        new_block1->shape = applyRightRotation(shape);

        Block *new_block2 = new Block;
        new_block2->shape = applyRightRotation(a);

        Block *new_block3 = new Block;
        new_block3->shape = applyRightRotation(b);

        new_block->right_rotation = new_block1;
        new_block1->left_rotation = new_block;

        new_block1->right_rotation = new_block;
        new_block->left_rotation = new_block1;

        new_block1->right_rotation = new_block2;
        new_block2->left_rotation = new_block1;

        new_block2->right_rotation = new_block3;
        new_block3->left_rotation = new_block2;

        new_block3->right_rotation = new_block;
        new_block->left_rotation = new_block3;
    }

    if (initial_block == nullptr) {
        initial_block = new_block;
        active_rotation = initial_block;
    } else {
        Block *lastBlock = initial_block;
        while (lastBlock->next_block != nullptr){
            lastBlock = lastBlock->next_block;
        }
        if (lastBlock->sizee == 1){
            lastBlock->next_block = new_block;
        } else if (lastBlock->sizee == 2){
            lastBlock->next_block = new_block;
            lastBlock->right_rotation->next_block = new_block;
        } else{
            lastBlock->next_block = new_block;
            lastBlock->right_rotation->next_block = new_block;
            lastBlock->right_rotation->right_rotation->next_block = new_block;
            lastBlock->right_rotation->right_rotation->right_rotation->next_block = new_block;
        }
    }
}

vector<vector<bool>> BlockFall::applyRightRotation(const vector<vector<bool>>& input) {

    int rows = input.size();
    int cols = input[0].size();
    vector<vector<bool>> right(cols, vector<bool>(rows, false));

    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            right[j][rows - (i + 1)] = input[i][j];
        }
    }
    return right;
}

void BlockFall::initialize_grid(const std::string &input_file) {
    // Open the input file
    std::ifstream infile(input_file);

    // Check if the file is open
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << input_file << std::endl;
        return;
    }

    // Read the first line to determine the number of columns
    std::string line;
    std::getline(infile, line);

    // Use a string stream to parse the first line and count the columns
    std::istringstream iss(line);
    cols = 0;
    int value;
    while (iss >> value) {
        cols++;
    }

    infile.clear();
    infile.seekg(0, std::ios::beg);

    // Count the number of rows in the file
    rows = 0;
    while (std::getline(infile, line)) {
        rows++;
    }

    // Reset the file pointer to the beginning
    infile.clear();
    infile.seekg(0, std::ios::beg);

    // Resize the grid vector
    grid.resize(rows, std::vector<int>(cols, 0));

    // Populate the grid with values from the file
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            infile >> grid[i][j];
        }
    }
    // Close the file
    infile.close();
}

int BlockFall::countOccupiedCells(Block* block) const {
    int count = 0;
    for (const auto& row : block->shape) {
        for (bool cell : row) {
            if (cell) {
                ++count;
            }
        }
    }
    return count;
}


void BlockFall::checkAndClearRows() {
    int rowsCleared = 0;

    for (int i = rows - 1; i >= 0; --i) {
        bool allOnes = true;
        for (int cell : grid[i]) {
            if (cell != 1) {
                allOnes = false;
                break;
            }
        }

        if (allOnes) {
            std::cout << "Before clearing:" << std::endl;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    std::cout << (grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
                }
                std::cout << std::endl;
            }
            std::cout << "\n" << std::endl;
        }

        while (allOnes) {
            // Clear the completed row
            grid.erase(grid.begin() + i);
            grid.insert(grid.begin(), std::vector<int>(cols, 0));
            ++rowsCleared;

            // Update allOnes for the next iteration
            allOnes = true;
            for (int cell : grid[i]) {
                if (cell != 1) {
                    allOnes = false;
                    break;
                }
            }
        }
    }

    if (rowsCleared != 0) {
        // Award points for cleared rows
        int rowClearPoints = rowsCleared * cols;
        current_score += rowClearPoints;
    }
}

void BlockFall::drop() {
    if (active_rotation == nullptr) {
        std::cerr << "Error: No active block to drop." << std::endl;
        return;
    }
    if (hasCollision(active_rotation, 0, 0)) {
        std::cout << "GAME OVER!\nNext block that couldn't fit:" << std::endl;
        for (const auto& row : active_rotation->shape) {
            for (bool cell : row) {
                std::cout << (cell ? occupiedCellChar : unoccupiedCellChar);
            }
            std::cout << std::endl;
        }
        std::cout << "\nFinal grid and score:" << std::endl;
        gameOver();
        won = -1;
        return;
    }

    checkAndClearRows();
    // Gravity is OFF, drop the block intact as far as possible
    if (!gravity_mode_on) {
        int temp_score=0;
        while (isValidMove(active_rotation, active_rotation->currentX, active_rotation->currentY, 0, 1)) {
            moveBlock(active_rotation, 0, 1);
            temp_score++;
        }
        updateGrid(active_rotation);

        // Calculate and award points for the block drop
        int blockPoints = temp_score * countOccupiedCells(active_rotation);
        current_score += blockPoints;
        checkAndClearRows();
    }

        // Gravity is ON, handle individual cell falling
    else {

        int temp=0;
        int temp_score=rows;
        for (int i=active_rotation->shape.size()-1; i>=0; i--) {
            for (int j=active_rotation->shape[i].size()-1; j>=0 ; j--) {
                if (active_rotation->shape[i][j]) {
                    int fallDistance = 0;
                    temp++;
                    while (isValidMoveGra(active_rotation, active_rotation->currentX,active_rotation->currentY + fallDistance, 0, 1, j, i)) {
                        fallDistance++;
                    }
                    if (fallDistance < temp_score) {
                        temp_score = fallDistance;
                    }

                    grid[active_rotation->currentY + i][active_rotation->currentX + j] = 0;
                    // Set the new cell in the grid
                    grid[active_rotation->currentY + fallDistance + i][active_rotation->currentX + j] = 1;
                }
            }
        }

        int blockPoints = temp_score * countOccupiedCells(active_rotation);
        current_score += blockPoints;
        checkAndClearRows();
    }
    if (isPowerUpActivated()) {
        // Activate power-up: clear the entire grid and award points
        int powerUpPoints = 1000;
        for (int i=0; i<rows; i++){
            for (int j=0; j<cols; j++){
                if (grid[i][j]==1) current_score++;
            }
        }
        current_score += powerUpPoints;
        std::cout << "Before clearing:" << std::endl;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                // Print the grid cell
                std::cout << (grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
            }
            std::cout << std::endl;
        }
        std::cout << "\n" << std::endl;
        // Clear the entire grid
        grid = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));
    }

    if (active_rotation->next_block != nullptr){
        active_rotation = active_rotation->next_block;
    } else {
        std::cout << "YOU WIN!\nNo more blocks.\nFinal grid and score:\n" << std::endl;
        won = 1;
        gameOver();
        return;
    }
    if (hasCollision(active_rotation, 0, 0)) {
        std::cout << "GAME OVER!\nNext block that couldn't fit:" << std::endl;
        for (const auto& row : active_rotation->shape) {
            for (bool cell : row) {
                std::cout << (cell ? occupiedCellChar : unoccupiedCellChar);
            }
            std::cout << std::endl;
        }
        std::cout << "\nFinal grid and score:" << std::endl;
        gameOver();
        won = -1;
        return;
    }
}

bool BlockFall::hasCollision(Block* block, int dx, int dy) const {
    for (int i = 0; i < block->shape.size(); ++i) {
        for (int j = 0; j < block->shape[i].size(); ++j) {
            if (block->shape[i][j] && grid[block->currentY + i + dy][block->currentX + j + dx] == 1) {
                return true;
            }
        }
    }
    return false;
}

bool BlockFall::isValidMoveGra(Block* block, int curX, int curY, int dx, int dy, int j, int i) const {
    if (block == nullptr) {
        std::cerr << "Error: Invalid block." << std::endl;
        return false;
    }

    // Calculate the new position after the move
    int newX = curX + j + dx;
    int newY = curY + i + dy;

    // Check if the new position is within the bounds of the grid
    if (newX < 0 || newY < 0 || newX >= cols || newY >= rows) {
        return false; // Move is out of bounds
    }

    // Check if the new position collides with an occupied cell on the grid
    if (grid[newY][newX] == 1) {
        return false; // Collision with an occupied cell
    }

    return true; // The move is valid
}


bool BlockFall::isValidMove(Block* block, int curX, int curY, int dx, int dy) const {
    if (block == nullptr) {
        std::cerr << "Error: Invalid block." << std::endl;
        return false;
    }

    // Iterate through the shape of the block
    for (int i=block->shape.size()-1; i>=0; i--) {
        for (int j=block->shape[i].size()-1; j>=0 ; j--) {
            if (block->shape[i][j]) {
                // Calculate the new position after the move
                int newX = curX + j + dx;
                int newY = curY + i + dy;

                // Check if the new position is within the bounds of the grid
                if (newX < 0 || newY < 0 || newX >= cols || newY >= rows) {
                    return false; // Move is out of bounds
                }

                // Check if the new position collides with an occupied cell on the grid
                if (grid[newY][newX] == 1) {
                    return false; // Collision with an occupied cell
                }
            }
        }
    }
    return true; // The move is valid
}
void BlockFall::updateGrid(Block *block) {
    for (int i=block->shape.size()-1; i>=0; i--) {
        for (int j=block->shape[i].size()-1; j>=0 ; j--) {
            if (block->shape[i][j]) {
                grid[active_rotation->currentY + i][active_rotation->currentX + j] = 1;
            }
        }
    }
}

void BlockFall::moveBlock(Block* block, int dx, int dy) {
    if (block == nullptr) {
        std::cerr << "Error: Invalid block." << std::endl;
        return;
    }

    // Update the current position of the block
    active_rotation->currentX += dx;
    active_rotation->currentY += dy;
}

void BlockFall::moveRight(){
    if (isValidMove(active_rotation, active_rotation->currentX,active_rotation->currentY, 1, 0)){
        active_rotation->currentX++;
    }
}

void BlockFall::moveLeft(){
    if (isValidMove(active_rotation, active_rotation->currentX,active_rotation->currentY, -1, 0)){
        active_rotation->currentX--;
    }
}

bool BlockFall::isRotationValid(Block* oldRotation, Block* newRotation) const {
    // Check if the new rotation fits within the grid
    if (newRotation->currentX < 0 || newRotation->currentY < 0 ||
        newRotation->currentX + newRotation->shape[0].size() > cols ||
        newRotation->currentY + newRotation->shape.size() > rows) {
        return false;
    }

    // Check if the new rotation collides with occupied cells on the grid
    for (int i = 0; i < newRotation->shape.size(); ++i) {
        for (int j = 0; j < newRotation->shape[i].size(); ++j) {
            if (newRotation->shape[i][j]) {
                int newX = newRotation->currentX + j;
                int newY = newRotation->currentY + i;

                if (newX < 0 || newY < 0 || newX >= cols || newY >= rows || grid[newY][newX] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

void BlockFall::rotateRight() {
    // Save the current active_rotation
    Block* previousRotation = active_rotation;

    // Move to the right rotated block
    active_rotation = active_rotation->right_rotation;
    active_rotation->currentY = previousRotation->currentY;
    active_rotation->currentX = previousRotation->currentX;

    // Check if the rotation is possible
    if (!isRotationValid(previousRotation, active_rotation)) {
        // Revert to the previous rotation
        active_rotation = previousRotation;
        return;
    }
    // Perform other rotation-related tasks (e.g., updating visuals, etc.) if needed
}

void BlockFall::rotateLeft() {
    // Save the current active_rotation
    Block* previousRotation = active_rotation;

    // Move to the left rotated block
    active_rotation = active_rotation->left_rotation;
    active_rotation->currentY = previousRotation->currentY;
    active_rotation->currentX = previousRotation->currentX;

    // Check if the rotation is possible
    if (!isRotationValid(previousRotation, active_rotation)) {
        // Revert to the previous rotation
        active_rotation = previousRotation;
        return;
    }
    // Perform other rotation-related tasks (e.g., updating visuals, etc.) if needed
}


bool BlockFall::isPartOfActiveBlock(int i, int j) const {
    // Check if the given coordinates (x, y) are part of the active rotation block
    if (active_rotation != nullptr &&
        i >= active_rotation->currentY &&
        i < active_rotation->currentY + active_rotation->shape.size() &&
        j >= active_rotation->currentX &&
        j < active_rotation->currentX + active_rotation->shape[0].size() &&
        active_rotation->shape[i - active_rotation->currentY][j - active_rotation->currentX]) {
        return true;
    }
    return false;
}

void BlockFall::toggleGravity() {
    gravity_mode_on = !gravity_mode_on;

    // If gravity is turned on, simulate a drop to rearrange cells
    if (gravity_mode_on) {
        // Iterate through each cell in the grid
        for (int i = rows - 2; i >= 0; --i) {
            for (int j = 0; j < cols; ++j) {
                // Check if the current cell is occupied
                if (grid[i][j] == 1 && grid[i+1][j] == 0) {
                    int fallDistance = 0;

                    // Find the maximum distance the cell can fall
                    while (i + fallDistance < rows - 1 && grid[i + fallDistance + 1][j] == 0 && !isPartOfActiveBlock(i, j)) {
                        ++fallDistance;
                    }

                    // Move the cell to the new position
                    grid[i + fallDistance][j] = 1;
                    grid[i][j] = 0;
                }
            }
        }
    }
    int rowsCleared = 0;

    for (int i = rows - 1; i >= 0; --i) {
        bool allOnes = true;
        for (int cell : grid[i]) {
            if (cell != 1) {
                allOnes = false;
                break;
            }
        }

        while (allOnes) {
            // Clear the completed row
            grid.erase(grid.begin() + i);
            grid.insert(grid.begin(), std::vector<int>(cols, 0));
            ++rowsCleared;

            // Update allOnes for the next iteration
            allOnes = true;
            for (int cell : grid[i]) {
                if (cell != 1) {
                    allOnes = false;
                    break;
                }
            }
        }
    }

    if (rowsCleared != 0) {
        // Award points for cleared rows
        int rowClearPoints = rowsCleared * cols;
        current_score += rowClearPoints;
    }
}


void BlockFall::gameOver() const {
    std::cout << "Score: " << current_score << std::endl;
    if (leaderboard.head_leaderboard_entry != nullptr){
        if (leaderboard.head_leaderboard_entry->score >= current_score) {
            std::cout << "High Score: " << leaderboard.head_leaderboard_entry->score << std::endl;
        } else {
            std::cout << "High Score: " << current_score << std::endl;
        }
    } else {
        std::cout << "High Score: " << current_score << std::endl;
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Print the grid cell
            std::cout << (grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
        }
        std::cout << std::endl;
    }
}

void BlockFall::printGrid() const {

    std::cout << "Score: " << current_score << std::endl;
    if (leaderboard.head_leaderboard_entry != nullptr){
        if (leaderboard.head_leaderboard_entry->score >= current_score) {
            std::cout << "High Score: " << leaderboard.head_leaderboard_entry->score << std::endl;
        } else {
            std::cout << "High Score: " << current_score << std::endl;
        }
    } else {
        std::cout << "High Score: " << current_score << std::endl;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Check if the current cell is part of the active block
            if (active_rotation != nullptr &&
                i >= active_rotation->currentY &&
                i < active_rotation->currentY + active_rotation->shape.size() &&
                j >= active_rotation->currentX &&
                j < active_rotation->currentX + active_rotation->shape[0].size() &&
                active_rotation->shape[i - active_rotation->currentY][j - active_rotation->currentX] &&
                !hasCollision(active_rotation, 0, 0)) {
                // Print the active block cell
                std::cout << occupiedCellChar;
            } else {
                // Print the grid cell
                std::cout << (grid[i][j] == 1 ? occupiedCellChar : unoccupiedCellChar);
            }
        }
        std::cout << std::endl;
    }
    std::cout << "\n" << std::endl;
}

bool BlockFall::isPowerUpActivated() const {
    // Define the region where the power-up is expected to be
    int powerUpRows = power_up.size();
    int powerUpCols = power_up[0].size();

    // Iterate over the region and check for matching cells
    for (int i = 0; i <= rows - powerUpRows; ++i) {
        for (int j = 0; j <= cols - powerUpCols; ++j) {
            bool powerUpMatch = true;
            for (int k = 0; k < powerUpRows; ++k) {
                for (int l = 0; l < powerUpCols; ++l) {
                    if (power_up[k][l] != (grid[i + k][j + l] == 1)) {
                        powerUpMatch = false;
                        break;
                    }
                }
                if (!powerUpMatch) {
                    break;
                }
            }
            if (powerUpMatch) {
                return true;
            }
        }
    }
    return false;
}

BlockFall::~BlockFall() {
    // Delete all blocks and their rotations
    Block *currentBlock = initial_block;
    while (currentBlock != nullptr) {
        Block *nextBlock = currentBlock->next_block;

        // Delete all rotations of the current block
        Block *rotation = currentBlock;
        do {
            Block *nextRotation = rotation->right_rotation;

            rotation->shape.clear();

            // Delete the rotation block
            delete rotation;

            rotation = nextRotation;
        } while (rotation != currentBlock);

        currentBlock = nextBlock;
    }
    initial_block = nullptr;
    active_rotation = nullptr;
    last_block = nullptr;
    // Clear the power-up
    deletePowerUp();
    grid.clear();
}



void BlockFall::deletePowerUp() {
    for (auto& row : power_up) {
        row.clear();
    }
    power_up.clear();
}
