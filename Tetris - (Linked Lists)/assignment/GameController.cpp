#include "GameController.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>

bool GameController::play(BlockFall& game, const string& commands_file) {
    // Open the commands file for reading
    std::ifstream infile(commands_file);
    if (!infile.is_open()) {
        cerr << "Error: Unable to open the commands file." << endl;
        return false;
    }

    // Read all commands into a vector
    std::vector<std::string> commands{
            std::istream_iterator<std::string>(infile),
            std::istream_iterator<std::string>()
    };

    // Close the file as it's no longer needed
    infile.close();

    // Process each command
    for (const auto& command : commands) {
        if (command == "PRINT_GRID") {
            game.printGrid();
        } else if (command == "ROTATE_RIGHT") {
            game.rotateRight();
        } else if (command == "ROTATE_LEFT") {
            game.rotateLeft();
        } else if (command == "MOVE_RIGHT") {
            game.moveRight();
        } else if (command == "MOVE_LEFT") {
            game.moveLeft();
        } else if (command == "DROP") {
            game.drop();
        } else if (command == "GRAVITY_SWITCH") {
            game.toggleGravity();
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
        if (game.won == -1) {
            LeaderboardEntry* new_entry = new LeaderboardEntry(game.current_score, time(nullptr), game.player_name);
            game.leaderboard.insert_new_entry(new_entry);
            game.leaderboard.write_to_file(game.leaderboard_file_name);
            game.leaderboard.print_leaderboard();
            return false;
        } else if (game.won == 1) {
            LeaderboardEntry* new_entry = new LeaderboardEntry(game.current_score, time(nullptr), game.player_name);
            game.leaderboard.insert_new_entry(new_entry);
            game.leaderboard.write_to_file(game.leaderboard_file_name);
            game.leaderboard.print_leaderboard();
            return true;
        }
    }

    cout << "GAME FINISHED!\nNo more commands.\nFinal grid and score:\n" << endl;
    game.gameOver();
    LeaderboardEntry* new_entry = new LeaderboardEntry(game.current_score, time(nullptr), game.player_name);
    game.leaderboard.insert_new_entry(new_entry);
    game.leaderboard.write_to_file(game.leaderboard_file_name);
    game.leaderboard.print_leaderboard();
    return true;
}