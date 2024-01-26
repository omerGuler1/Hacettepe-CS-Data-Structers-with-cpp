#include "EncodeMessage.h"
#include <cmath>
#include <iostream>


// Default Constructor
EncodeMessage::EncodeMessage() {

}

// Destructor
EncodeMessage::~EncodeMessage() {

}

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix& img, const std::string& message, const std::vector<std::pair<int, int>>& positions) {

    ImageMatrix encodedImage(img);

    // Step 1: edit the message
    std::string transformedMessage = message;
    for (int i = 0; i < transformedMessage.length(); i++) {
        // Check if the index is prime
        if (isPrime(i)) {
            // Get the corresponding Fibonacci number and add to the ASCII value of corresponding char
            int asciiValue = transformedMessage[i] + getFibonacciNumber(i);

            // Ensure printable characters by adjusting the ASCII value
            if (asciiValue <= 32) {
                asciiValue += 33;
            } else if (asciiValue >= 127) {
                asciiValue = 126;
            }

            // Update the char
            transformedMessage[i] = static_cast<char>(asciiValue);
        }
    }

    // Step 2: Right Circular Shifting
    transformedMessage = rightCircularShift(transformedMessage);


    // Step 3: Message Embedding
    std::string binaryMessage;  // Convert the message to a binary string
    binaryMessage = messageToBinary(transformedMessage);

    int binaryIndex = 0;

    for (const auto& position : positions) {

        int x = position.first;
        int y = position.second;
        if (x >= 0 && x < encodedImage.get_height() && y >= 0 && y < encodedImage.get_width()) {
            // Look at the boundaries
            if (binaryIndex < binaryMessage.length()) {
                // Get the original pixel value
                int pixelValue = img.get_data(x, y);

                // Replace the original pixel value with the new one
                int newPixelValue = (pixelValue & 254) | (binaryMessage[binaryIndex] - '0');

                // Update the pixel value
                encodedImage.set_data(x, y, static_cast<unsigned char>(newPixelValue));

                // Move to the next bit in the binary message
                binaryIndex++;
            }
        }
    }

    return encodedImage;
}


bool EncodeMessage::isPrime(int k) {
    if (k <= 1) {
        return false;
    } else if (k==2){
        return true;
    }

    for (int i = 2; i * i <= k; i++) {
        if (k % i == 0) {
            return false;
        }
    }
    return true;
}

int EncodeMessage::getFibonacciNumber(int n) {
    if (n <= 1) {
        return n;
    }

    int prev = 0;
    int current = 1;
    int next;

    for (int i = 2; i <= n; i++) {
        next = prev + current;
        prev = current;
        current = next;
    }

    return next;
}

std::string EncodeMessage::rightCircularShift(std::string transformedMessage){

    std::string shiftedMessage = transformedMessage;
    int length = transformedMessage.length();

    for (size_t i = 0; i < transformedMessage.length(); i++) {
        int index = (i + (length / 2)) % length;
        shiftedMessage[index] = transformedMessage[i];
    }
    return shiftedMessage;
}

// first messageToBinary function i wrote (which is used to wrong but now graded updated and this is also correct)

std::string EncodeMessage::messageToBinary(const std::string& str) {
    std::string binaryMessage = "";

    for (size_t i = 0; i < str.length(); i++) {
        int asciiValue = str[i];

        std::string binaryChar;
        for (int j = 6; j >= 0; j--) {
            binaryChar += ((asciiValue >> j) & 1) ? '1' : '0';
        }

        binaryMessage += binaryChar;
    }
    return binaryMessage;
}