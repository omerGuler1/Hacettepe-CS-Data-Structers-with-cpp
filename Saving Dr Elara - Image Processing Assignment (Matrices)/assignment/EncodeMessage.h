#ifndef ENCODE_MESSAGE_H
#define ENCODE_MESSAGE_H

#include <string>
#include <vector>
#include "ImageMatrix.h"

class EncodeMessage {
public:
    EncodeMessage();
    ~EncodeMessage();

    ImageMatrix encodeMessageToImage(const ImageMatrix &img, const std::string &message, const std::vector<std::pair<int, int>>& positions);


private:

    static bool isPrime(int i);
    static int getFibonacciNumber(int i);
    static std::string rightCircularShift(std::string transformedMessage);

    std::string messageToBinary(const std::string &str);
};

#endif // ENCODE_MESSAGE_H
