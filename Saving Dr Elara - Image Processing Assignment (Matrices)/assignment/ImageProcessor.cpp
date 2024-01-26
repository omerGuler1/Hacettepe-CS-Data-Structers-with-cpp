#include <iostream>
#include <utility>
#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() {}

ImageProcessor::~ImageProcessor() {

}


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    ImageSharpening sharpImage0;
    ImageMatrix sharpImage = sharpImage0.sharpen(img, 2);

    EdgeDetector findEdges;
    std::vector<std::pair<int, int>> edges = findEdges.detectEdges(sharpImage);

    DecodeMessage obj;
    std::string text = obj.decodeFromImage(sharpImage, edges);
    return text;
}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {
    ImageSharpening sharpImage0;
    ImageMatrix sharpImage = sharpImage0.sharpen(img, 2);

    EdgeDetector findEdges;
    std::vector<std::pair<int, int>> edges = findEdges.detectEdges(sharpImage);

    EncodeMessage obj;
    ImageMatrix encodedImage = obj.encodeMessageToImage(img, message, edges);

    return encodedImage;
}