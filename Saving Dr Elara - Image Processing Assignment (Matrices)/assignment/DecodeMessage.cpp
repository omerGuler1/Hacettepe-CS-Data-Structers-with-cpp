// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>

// Default constructor
DecodeMessage::DecodeMessage() {
    // Nothing specific to initialize here
}

// Destructor
DecodeMessage::~DecodeMessage() {
    // Nothing specific to clean up
}

std::string DecodeMessage::decodeFromImage(const ImageMatrix& image, const std::vector<std::pair<int, int>>& edgePixels) {
    std::string binaryString;
    std::string message;

    // Step 1: Extract lsb from edges
    for (const auto& edgePixel : edgePixels) {
        int x = edgePixel.first;
        int y = edgePixel.second;
        if (x >= 0 && x < image.get_height() && y >= 0 && y < image.get_width()) {
            // Extract the least significant bit from the pixel and append to create binary string
            int lsb = static_cast<int>(image.get_data(x, y)) & 1;
            binaryString += std::to_string(lsb);
        }
    }

    // Step 2: Converting Binary to ASCII

    // pad string so it can be divided by 7
    while (binaryString.length() % 7 != 0) {
        binaryString = "0" + binaryString;
    }

    // Convert each 7-bit segment into its corresponding decimal value
    for (size_t i = 0; i < binaryString.length(); i += 7) {
        std::string segment = binaryString.substr(i, 7);
        int decimalValue = std::stoi(segment, nullptr, 2);

        // adjust ascii value
        if (decimalValue <= 32) {
            decimalValue += 33;
        } else if (decimalValue >= 127) {
            decimalValue = 126;
        }
        char character = static_cast<char>(decimalValue);
        message += character;
    }

    return message;
}