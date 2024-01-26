#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

class Network {
public:
    Network();
    ~Network();

    vector<Client> clis;


    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    std::string getActivityTypeString(ActivityType type) {
        switch (type) {
            case ActivityType::MESSAGE_FORWARDED:
                return "Message Forwarded";
            case ActivityType::MESSAGE_SENT:
                return "Message Sent";
            case ActivityType::MESSAGE_DROPPED:
                return "Message Dropped";
            case ActivityType::MESSAGE_RECEIVED:
                return "Message Received";
            default:
                return "Unknown Activity Type";
        }
    }


    void showQInfo(std::vector<std::string> tokens, vector<Client> &clients);
    void showFrameInfo(std::vector<std::string> tokens, vector<Client> &clients);
    void messageF(std::vector<std::string> tokens, vector<Client> &clients, int message_limit, std::string message, const string &sender_port, const string &receiver_port);

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename); 
};

#endif  // NETWORK_H
