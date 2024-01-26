// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>
#include <iostream>

// Default constructor
EdgeDetector::EdgeDetector() {
    Gx = new double*[3];
    Gy = new double*[3];

    for (int i = 0; i < 3; i++) {
        Gx[i] = new double[3];
        Gy[i] = new double[3];
    }

    Gx[0][0] = -1.0;
    Gx[0][1] = 0.0;
    Gx[0][2] = 1.0;
    Gx[1][0] = -2.0;
    Gx[1][1] = 0.0;
    Gx[1][2] = 2.0;
    Gx[2][0] = -1.0;
    Gx[2][1] = 0.0;
    Gx[2][2] = 1.0;

    Gy[0][0] = -1.0;
    Gy[0][1] = -2.0;
    Gy[0][2] = -1.0;
    Gy[1][0] = 0.0;
    Gy[1][1] = 0.0;
    Gy[1][2] = 0.0;
    Gy[2][0] = 1.0;
    Gy[2][1] = 2.0;
    Gy[2][2] = 1.0;
}


// Destructor
EdgeDetector::~EdgeDetector() {

}

// detect edges
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix& input_image) {

    Convolution obj1(Gx, 3, 3, 1, true);
    Convolution obj2(Gy, 3, 3, 1, true);

    ImageMatrix Ix = obj1.convolve(input_image);
    ImageMatrix Iy = obj2.convolve(input_image);

    ImageMatrix magnitude(input_image.get_height(), input_image.get_width());

    // Calculate the average gradient magnitude with all pixels
    double threshold = 0.0;
    int totalPixels = input_image.get_width() * input_image.get_height();

    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            double Ix_value = Ix.get_data(i, j);
            double Iy_value = Iy.get_data(i, j);
            double gradient_magnitude = sqrt(Ix_value * Ix_value + Iy_value * Iy_value);

            magnitude.set_data(i, j, gradient_magnitude);
            threshold += gradient_magnitude;
        }
    }

    threshold /= totalPixels;

    // a vector to store edge pixel coordinates
    std::vector<std::pair<int, int>> edgePixels;

    for (int i = 0; i < input_image.get_height(); i++) {
        for (int j = 0; j < input_image.get_width(); j++) {
            if (magnitude.get_data(i, j) > threshold) {
                // This pixel is on an edge
                edgePixels.emplace_back(i, j);
            }
        }
    }

    return edgePixels;
}
