#include "Network.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Network::Network() {}


void Network::process_commands(vector<Client> &clients,
                               vector<string> &commands,
                               int message_limit,
                               const string &sender_port,
                               const string &receiver_port) {

    for (const auto &command_line : commands) {
        // Parse the command_line into tokens based on spaces
        std::istringstream iss(command_line);
        std::vector<std::string> tokens;
        std::string message;
        std::string token;
        int count=0;


        // tokens: parsed of MESSAGE<space>sender_ID<space>receiver_ID<space>#message#

        while (iss >> token) {
            if (!token.empty() && token[0] == '#') {

                if(token.back() == '#'){
                    token = token.substr(0, token.length()-1);
                    message += token.substr(1);
                } else {
                    message += token.substr(1) + " ";
                }


                count++;
            } else if(!token.empty() && token.back() == '#') {
                token = token.substr(0, token.length()-1);
                message += token;
                count++;
            } else if (count==1){
                message += token + " ";
            } else {
                tokens.push_back(token);
            }
            if (count==2){
                tokens.push_back(message);
            }
        }

        for (string element : tokens){
            for (int i=0; i<command_line.size()+9; i++){
                std::cout << "-";
            }
            std::cout << std::endl;
            std::cout << "Command: " << command_line << std::endl;
            for (int i=0; i<command_line.size()+9; i++){
                std::cout << "-";
            }
            std::cout << std::endl;
            if (element == "MESSAGE") {
                messageF(tokens,clients, message_limit, message, sender_port, receiver_port);
            } else if (element == "SHOW_FRAME_INFO") {
                showFrameInfo(tokens,clients);
            } else if (element == "SHOW_Q_INFO") {
                showQInfo(tokens, clients);
            } else if (element == "SEND") {

                for (Client& sender : clients) {
                    string mess;
                    int count1=0;
                    while (!sender.outgoing_queue.empty()) {

                        count1++;
                        stack<Packet*> frame = sender.outgoing_queue.front();
                        string intendedID;
                        string senderID;
                        stack<Packet*> temp = frame;

                        while (!temp.empty()) {
                            if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(temp.top())) {
                                intendedID = appLayerPacket->receiver_ID;
                                senderID = appLayerPacket->sender_ID;
                            }
                            temp.pop();
                        }

                        string hopID;
                        hopID = sender.routing_table[intendedID];

                        bool found = false;
                        for (const Client& obj : clients) {
                            if (obj.client_id == hopID) {
                                found = true;
                                break;
                            }
                        }
                        if (found){
                            for (Client& hop : clients){
                                if (hop.client_id == hopID){

                                    Log log1 = Log(getCurrentTimestamp(), "", count1, 0,
                                                   senderID, intendedID,true,ActivityType::MESSAGE_FORWARDED);


                                    hop.incoming_queue.push(frame);

                                    stack<Packet*> temp1 = frame;

                                    if(frame.top()->frameCount ==1){
                                        mess = "";
                                        count1=1;
                                        hop.log_entries.push_back(log1);
                                    }

                                    while (!temp1.empty()) {
                                        if (ApplicationLayerPacket *appLayerPacket = dynamic_cast<ApplicationLayerPacket *>(temp1.top())) {
                                            mess += appLayerPacket->message_data;
                                        }
                                        temp1.pop();
                                    }

                                    if (hop.client_id == intendedID) {
                                        log1.message_content = mess;
                                        log1.activity_type = ActivityType::MESSAGE_RECEIVED;
                                        hop.log_entries.back() = log1;
                                    }

                                    sender.outgoing_queue.pop();

                                    std::cout << "Client " << sender.client_id << " sending frame #"
                                              << frame.top()->frameCount << " to client "
                                              << hop.client_id << std::endl;

                                    stack<Packet *> tempStack = frame;
                                    while (!tempStack.empty()) {
                                        tempStack.top()->print();
                                        tempStack.pop();
                                    }

                                    frame.top()->hopCount++;
                                    log1.number_of_hops = frame.top()->hopCount;
                                    log1.number_of_frames = count1;
                                    hop.log_entries.back() = log1;

                                    std::cout << "Number of hops so far: " << log1.number_of_hops
                                              << std::endl;
                                    std::cout << "--------" << std::endl;
                                }
                            }
                        } else {
                            // sender.outgoing_queue.pop();

                            stack<Packet*> currentFrame = sender.outgoing_queue.front();
                            while (!currentFrame.empty()) {
                                delete currentFrame.top();
                                currentFrame.top() = nullptr;
                                currentFrame.pop();
                            }
                            sender.outgoing_queue.pop();
                        }
                    }
                }
                clis = clients;
            }  else if (element == "RECEIVE") {

                for (Client& receiver : clients) {
                    string mess;
                    while (!receiver.incoming_queue.empty()) {

                        stack<Packet*> receivedFrame = receiver.incoming_queue.front();
                        string intendedID;
                        string senderID;
                        stack<Packet*> temp = receivedFrame;

                        while (!temp.empty()) {
                            if (ApplicationLayerPacket* appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(temp.top())) {
                                intendedID = appLayerPacket->receiver_ID;
                                senderID = appLayerPacket->sender_ID;
                                mess += appLayerPacket->message_data;
                            }
                            temp.pop();
                        }

                        string hopID;
                        bool dropped = true;
                        for (const auto& entry : receiver.routing_table){
                            for (Client& hop : clients){
                                if (hop.client_id == entry.second && entry.first == intendedID) {
                                    hopID = entry.second;
                                    dropped = false;
                                }
                            }
                        }

                        if (receiver.client_id == intendedID){
                            if (!receiver.incoming_queue.empty()){
                                stack<Packet*> receivedFrame1 = receiver.incoming_queue.front();
                                stack<Packet*> temp1 = receivedFrame1;

                                PhysicalLayerPacket* fra = dynamic_cast<PhysicalLayerPacket *>(receivedFrame1.top());

                                for (Client& cli : clients){
                                    if (cli.client_mac == fra->sender_MAC_address){
                                        std::cout << "Client " << intendedID << " receiving frame #" << receivedFrame1.top()->frameCount <<
                                                  " from client " << cli.client_id << ", originating from client " << senderID << std::endl;
                                    }
                                }

                                while (!temp1.empty()) {
                                    if (PhysicalLayerPacket* packet = dynamic_cast<PhysicalLayerPacket*>(temp1.top())) {
                                        packet->print();
                                    } else if (NetworkLayerPacket* packet = dynamic_cast<NetworkLayerPacket*>(temp1.top())) {
                                        packet->print();
                                    } else if (TransportLayerPacket* packet = dynamic_cast<TransportLayerPacket*>(temp1.top())) {
                                        packet->print();
                                    } else if (ApplicationLayerPacket* packet = dynamic_cast<ApplicationLayerPacket*>(temp1.top())) {
                                        packet->print();
                                    }
                                    temp1.pop();
                                }
                                std::cout << "Number of hops so far: " << receivedFrame1.top()->hopCount << std::endl;
                                std::cout << "--------" << std::endl;
                                // receiver.incoming_queue.pop();

                                stack<Packet*> currentFrame = receiver.incoming_queue.front();
                                while (!currentFrame.empty()) {
                                    delete currentFrame.top();
                                    currentFrame.top() = nullptr;
                                    currentFrame.pop();
                                }
                                receiver.incoming_queue.pop();

                                if (!receiver.incoming_queue.empty()){
                                    stack<Packet*> receivedFrame2 = receiver.incoming_queue.front();

                                    if (receivedFrame2.top()->frameCount == 1){
                                        std::cout << "Client " << intendedID << " received the message \"" << mess << "\" from client " << senderID << "." << std::endl;
                                        std::cout << "--------" << std::endl;
                                        mess = "";
                                    }
                                } else {
                                    std::cout << "Client " << intendedID << " received the message \"" << mess << "\" from client " << senderID << "." << std::endl;
                                    std::cout << "--------" << std::endl;
                                    mess = "";
                                }
                            }

                        } else if (dropped){

                            for (Log& k : receiver.log_entries){
                                if (k.receiver_id == intendedID && k.sender_id == senderID){
                                    k.activity_type = ActivityType::MESSAGE_DROPPED;
                                    k.success_status = false;
                                }
                            }
                            if (!receiver.incoming_queue.empty()){
                                stack<Packet *> frame = receiver.incoming_queue.front();


                                std::cout << "Client " << receiver.client_id << " receiving frame #" << frame.top()->frameCount << " from client " << senderID << ", but intended for client " << intendedID << ". Forwarding..." << std::endl;
                                std::cout << "Error: Unreachable destination. Packets are dropped after 1 hops!" << std::endl;

                                receiver.incoming_queue.pop();

                                if (!receiver.incoming_queue.empty()){
                                    stack<Packet*> receivedFrame2 = receiver.incoming_queue.front();

                                    if (receivedFrame2.top()->frameCount == 1){
                                        std::cout << "--------" << std::endl;
                                        mess = "";
                                    }
                                } else {
                                    std::cout << "--------" << std::endl;
                                    mess = "";
                                }
                                receiver.outgoing_queue.push(frame);
                            }
                        } else if (!dropped){

                            if (!receiver.incoming_queue.empty()) {

                                stack<Packet *> frame = receiver.incoming_queue.front();
                                PhysicalLayerPacket* fra = dynamic_cast<PhysicalLayerPacket *>(frame.top());
                                string b;
                                for (const Client& cli : clients){
                                    if (cli.client_mac == fra->sender_MAC_address)
                                        b = cli.client_id;
                                }
                                fra->sender_MAC_address = receiver.client_mac;

                                for (const Client& hop : clients){
                                    if (hop.client_id == hopID){
                                        fra->receiver_MAC_address = hop.client_mac;
                                    }
                                }

                                if (frame.top()->frameCount ==1){
                                    std::cout << "Client " << receiver.client_id << " receiving a message from client " << b << ", but intended for client " << intendedID << ". Forwarding..." << std::endl;
                                }

                                std::cout << "Frame #" << frame.top()->frameCount << " MAC address change: New sender MAC " << fra->sender_MAC_address << ", new receiver MAC " << fra->receiver_MAC_address << std::endl;
                                receiver.incoming_queue.pop();


                                if (!receiver.incoming_queue.empty()){
                                    stack<Packet*>  receivedFrame2 = receiver.incoming_queue.front();

                                    if (receivedFrame2.top()->frameCount == 1){
                                        std::cout << "--------" << std::endl;
                                        mess = "";
                                    }
                                } else {
                                    std::cout << "--------" << std::endl;
                                    mess = "";
                                }

                                receiver.outgoing_queue.push(frame);
                            }
                        }

                    }
                }
                clis = clients;
            } else if (element == "PRINT_LOG") {
                for (Client& client : clients) {
                    if (client.client_id == tokens[1] && !client.log_entries.empty()) {
                        std::cout << "Client " << client.client_id << " Logs:" << std::endl;

                        for (int i = 0; i < client.log_entries.size(); ++i) {
                            std::cout << "--------------" << std::endl;
                            std::cout << "Log Entry #" << i + 1 << ":" << std::endl;
                            std::cout << "Activity: " << getActivityTypeString(client.log_entries[i].activity_type) << std::endl;
                            std::cout << "Timestamp: " << client.log_entries[i].timestamp << std::endl;
                            std::cout << "Number of frames: " << client.log_entries[i].number_of_frames << std::endl;
                            std::cout << "Number of hops: " << client.log_entries[i].number_of_hops << std::endl;
                            std::cout << "Sender ID: " << client.log_entries[i].sender_id << std::endl;
                            std::cout << "Receiver ID: " << client.log_entries[i].receiver_id << std::endl;
                            std::cout << "Success: " << (client.log_entries[i].success_status ? "Yes" : "No") << std::endl;
                            if (!client.log_entries[i].message_content.empty()){
                                std::cout << "Message: \"" << client.log_entries[i].message_content << "\"" << std::endl;
                            }
                        }
                    }
                }
            }
            else {
                std::cout << "Invalid command." << std::endl;
            }
            break;
        }
    }
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
}

void Network::messageF(std::vector<std::string> tokens, vector<Client> &clients,
                       int message_limit, std::string message, const string &sender_port, const string &receiver_port){

    int totalLength = message.length();
    std::cout << "Message to be sent: \"" << message << "\"\n" << std::endl;


    for (Client& sender : clients){
        if (sender.client_id == tokens[1]){
            for (Client& receiver : clients){
                if (receiver.client_id == tokens[2]){
                    Log log1 = Log(getCurrentTimestamp(), message, 0, 0, sender.client_id, receiver.client_id, true, ActivityType::MESSAGE_SENT);
                    sender.log_entries.push_back(log1);
                    for (int i = 0; i < totalLength; i += message_limit) {
                        stack<Packet*> frame;
                        for (const auto& entry : sender.routing_table) {
                            if (entry.first == receiver.client_id){
                                for (const Client& hop : clients){
                                    if (hop.client_id == entry.second){
                                        std::string substring = message.substr(i, message_limit);
                                        Packet* appLayer = new ApplicationLayerPacket(0, sender.client_id, receiver.client_id, substring);
                                        appLayer->frameCount = (i/message_limit)+1;
                                        Packet* transLayer = new TransportLayerPacket(1, sender_port, receiver_port);
                                        transLayer->frameCount = (i/message_limit)+1;;
                                        Packet* netLayer = new NetworkLayerPacket(2, sender.client_ip, receiver.client_ip);
                                        netLayer->frameCount = (i/message_limit)+1;
                                        Packet* phyLayer = new PhysicalLayerPacket(3, sender.client_mac, hop.client_mac);
                                        phyLayer->frameCount = (i/message_limit)+1;
                                        frame.push(appLayer);
                                        frame.push(transLayer);
                                        frame.push(netLayer);
                                        frame.push(phyLayer);
                                        sender.outgoing_queue.push(frame);

                                        log1.number_of_frames++;
                                        sender.log_entries.back() = log1;

                                        std::cout << "Frame #" << (i/message_limit)+1 << std::endl;
                                        phyLayer->print();
                                        netLayer->print();
                                        transLayer->print();
                                        appLayer->print();
                                        std::cout << "Number of hops so far: " << log1.number_of_hops << std::endl;
                                        std::cout << "--------" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    clis = clients;
}


void Network::read_routing_tables(std::vector<Client> &clients, const std::string &filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::string line;
    int clientIndex = 0;

    while (std::getline(file, line)) {
        if (line == "-") {
            // Skip the separator line
            clientIndex++;
            continue;
        }

        std::istringstream iss(line);
        std::string destinationClientID, nearestNeighborClientID;

        while (iss >> destinationClientID >> nearestNeighborClientID) {
            // Assuming "B" is the routing information, modify as needed
            clients[clientIndex].routing_table[destinationClientID] = nearestNeighborClientID;
        }
    }
    clis = clients;
    file.close();
}


void Network::showQInfo(std::vector<std::string> tokens, vector<Client> &clients){

    int size=0;
    int size1=0;
    for (Client& client : clients){
        if (client.client_id == tokens[1]) {
            size = client.outgoing_queue.size();
            size1 = client.incoming_queue.size();
        }
    }

    if (tokens[2] == "out") {
        std::cout << "Client " << tokens[1] << " Outgoing" << " Queue Status" << std::endl;
        std::cout << "Current total number of frames: " << size << std::endl;
    } else if ((tokens[2] == "in")){
        std::cout << "Client " << tokens[1] << " Incoming" << " Queue Status" << std::endl;
        std::cout << "Current total number of frames: " << size1 << std::endl;
    }
}

void Network::showFrameInfo(std::vector<std::string> tokens, vector<Client> &clients){

    for (Client& client : clients){
        if (client.client_id == tokens[1]) {

            if (tokens[2] == "out") {
                queue<stack<Packet*>> temp_queue = client.outgoing_queue;

                if (temp_queue.size() < std::stoi(tokens[3])){
                    std::cout << "No such frame." << std::endl;
                    return;
                }

                for (int i=1; i < std::stoi(tokens[3]) ; i++) {
                    temp_queue.pop();
                }

                stack<Packet*> nth_stack = temp_queue.front();

                stack<Packet*> temp_stack;
                while (!nth_stack.empty()) {
                    temp_stack.push(nth_stack.top());
                    nth_stack.pop();
                }

                if (!temp_stack.empty()) {
                    std::cout << "Current Frame #" << tokens[3] << " on the outgoing queue of client " << tokens[1] << std::endl;

                    ApplicationLayerPacket* nth_packet4 = dynamic_cast<ApplicationLayerPacket *>(temp_stack.top());
                    std::cout << "Carried Message: \"" << nth_packet4->message_data << "\"" << std::endl;
                    std::cout << "Layer 0 info: " << "Sender ID: "<< nth_packet4->sender_ID <<", Receiver ID: " << nth_packet4->receiver_ID << std::endl;
                    temp_stack.pop();

                    TransportLayerPacket* nth_packet3 = dynamic_cast<TransportLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 1 info: ";
                    nth_packet3->print();
                    temp_stack.pop();

                    NetworkLayerPacket* nth_packet2 = dynamic_cast<NetworkLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 2 info: ";
                    nth_packet2->print();
                    temp_stack.pop();

                    PhysicalLayerPacket* nth_packet1 = dynamic_cast<PhysicalLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 3 info: ";
                    nth_packet1->print();
                    std::cout << "Number of hops so far: " << nth_packet1->hopCount << std::endl;
                    temp_stack.pop();

                }

            } else if ((tokens[2] == "in")){

                queue<stack<Packet*>> temp_queue = client.incoming_queue;

                if (temp_queue.size() < std::stoi(tokens[3])){
                    std::cout << "No such frame." << std::endl;
                    return;
                }

                for (int i=1; i < std::stoi(tokens[3]) ; i++) {
                    temp_queue.pop();
                }

                stack<Packet*> nth_stack = temp_queue.front();

                stack<Packet*> temp_stack;
                while (!nth_stack.empty()) {
                    temp_stack.push(nth_stack.top());
                    nth_stack.pop();
                }

                if (!temp_stack.empty()) {
                    std::cout << "Current Frame #" << tokens[3] << " on the incoming queue of client " << tokens[1] << std::endl;

                    ApplicationLayerPacket* nth_packet4 = dynamic_cast<ApplicationLayerPacket *>(temp_stack.top());
                    std::cout << "Carried Message: \"" << nth_packet4->message_data << "\"" << std::endl;
                    std::cout << "Layer 0 info: " << "Sender ID: "<< nth_packet4->sender_ID <<", Receiver ID: " << nth_packet4->receiver_ID << std::endl;
                    temp_stack.pop();

                    TransportLayerPacket* nth_packet3 = dynamic_cast<TransportLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 1 info: ";
                    nth_packet3->print();
                    temp_stack.pop();

                    NetworkLayerPacket* nth_packet2 = dynamic_cast<NetworkLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 2 info: ";
                    nth_packet2->print();
                    temp_stack.pop();

                    PhysicalLayerPacket* nth_packet1 = dynamic_cast<PhysicalLayerPacket *>(temp_stack.top());
                    std::cout << "Layer 3 info: ";
                    nth_packet1->print();
                    std::cout << "Number of hops so far: " << nth_packet1->hopCount << std::endl;
                    temp_stack.pop();
                }
            }
        }
    }
}
// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return commands;
    }

    int number_of_commands;
    file >> number_of_commands;
    file.ignore();  // Consume the newline character after the number_of_commands

    for (int i = 0; i < number_of_commands; ++i) {
        std::string command_line;
        std::getline(file, command_line);

        // Skip empty lines
        if (command_line.empty()) {
            --i;
            continue;
        }

        // Add the command line to the vector of commands
        commands.push_back(command_line);
    }

    file.close();

    return commands;
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return clients;
    }

    int numOfClients;
    file >> numOfClients;

    for (int i=0; i<numOfClients; i++){
        std::string client_ID, client_IP_Address, client_MAC_Address;
        file >> client_ID >> client_IP_Address >> client_MAC_Address;
        Client client(client_ID, client_IP_Address, client_MAC_Address);

        clients.push_back(client);
    }
    file.close();
    clis = clients;
    return clients;
}

Network::~Network() {
    for (Client& client : clis){
        while (!client.outgoing_queue.empty()) {
            stack<Packet*> currentFrame = client.outgoing_queue.front();
            while (!currentFrame.empty()) {
                delete currentFrame.top();
                currentFrame.pop();
            }
            client.outgoing_queue.pop();
        }
        while (!client.incoming_queue.empty()) {
            stack<Packet*> currentFrame = client.incoming_queue.front();
            while (!currentFrame.empty()) {
                delete currentFrame.top();
                currentFrame.pop();
            }
            client.incoming_queue.pop();
        }
    }
}


