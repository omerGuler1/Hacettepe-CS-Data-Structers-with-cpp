#include <iostream>
#include <fstream>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry* new_entry) {

    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score ||
        (new_entry->score == head_leaderboard_entry->score && new_entry->last_played < head_leaderboard_entry->last_played)) {
        // Insert at the beginning
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        LeaderboardEntry* current = head_leaderboard_entry;
        LeaderboardEntry* previous = nullptr;

        // Find the correct position
        while (current && (new_entry->score < current->score ||
                           (new_entry->score == current->score && new_entry->last_played >= current->last_played))) {
            previous = current;
            current = current->next_leaderboard_entry;
        }

        // Insert new_entry at the correct position
        previous->next_leaderboard_entry = new_entry;
        new_entry->next_leaderboard_entry = current;
    }

    // Trim the leaderboard to 10 entries
    LeaderboardEntry* last_entry = head_leaderboard_entry;
    LeaderboardEntry* previous_entry = nullptr;

    for (int i = 0; i < 10 && last_entry; ++i) {
        previous_entry = last_entry;
        last_entry = last_entry->next_leaderboard_entry;
    }
    while (last_entry) {
        LeaderboardEntry* temp = last_entry;
        last_entry = last_entry->next_leaderboard_entry;
        delete temp;
    }

    if (previous_entry) {
        previous_entry->next_leaderboard_entry = nullptr;
    } else {
        head_leaderboard_entry = nullptr;
    }
}


void Leaderboard::write_to_file(const std::string& filename) {
    // TODO: Write the latest leaderboard status to the given file in the specified format
    std::ofstream file(filename);
    if (!file) {
        std::cout << "Error writing leaderboard to file." << std::endl;
        return;
    }

    LeaderboardEntry* current = head_leaderboard_entry;
    while (current) {
        file << current->score << " " << current->last_played << " " << current->player_name << endl;
        current = current->next_leaderboard_entry;
    }

    file.close();
}

void Leaderboard::read_from_file(const string& filename) {
    ifstream file(filename);

    // Free existing leaderboard entries
    while (head_leaderboard_entry) {
        LeaderboardEntry* next = head_leaderboard_entry->next_leaderboard_entry;
        delete head_leaderboard_entry;
        head_leaderboard_entry = next;
    }

    // Read from the file and insert entries into the leaderboard
    unsigned long score;
    time_t last_played;
    string player_name;

    while (file >> score >> last_played >> player_name) {
        insert_new_entry(new LeaderboardEntry(score, last_played, player_name));
    }

    file.close();
}

void Leaderboard::print_leaderboard() {
    LeaderboardEntry* current = head_leaderboard_entry;
    int order = 1;

    std::cout << "\nLeaderboard" << std::endl;
    std::cout << "-----------" << std::endl;

    while (current) {
        std::cout << order << ". " << current->player_name << " " << current->score << " "
                  << getFormattedTime(current->last_played) << std::endl;

        current = current->next_leaderboard_entry;
        ++order;
    }
}

std::string Leaderboard::getFormattedTime(time_t timestamp) {
    struct tm* time_info;
    char buffer[80];

    time_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%H:%M:%S/%d.%m.%Y", time_info);

    return std::string(buffer);
}

Leaderboard::~Leaderboard() {
    while (head_leaderboard_entry) {
        LeaderboardEntry* next = head_leaderboard_entry->next_leaderboard_entry;
        delete head_leaderboard_entry;
        head_leaderboard_entry = next;
    }
}