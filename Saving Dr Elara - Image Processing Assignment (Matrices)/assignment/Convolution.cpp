#include <iostream>
#include "Convolution.h"

// Default constructor 
Convolution::Convolution() {
}

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double** customKernel, int kh, int kw, int stride_val, bool pad){
    kernelH = kh;
    kernelW = kw;
    strideValue = stride_val;
    padding = pad;

    kernel = new double*[kernelH];
    for (int i = 0; i < kernelH; ++i) {
        kernel[i] = new double[kernelW];
        for (int j = 0; j < kernelW; ++j) {
            kernel[i][j] = customKernel[i][j];
        }
    }
}

// Destructor
Convolution::~Convolution() {
    if (kernel != nullptr) {
        for (int i = 0; i < kernelH; ++i) {
            delete[] kernel[i];
        }
        delete[] kernel;
    }
}

// Copy constructor
Convolution::Convolution(const Convolution &other){

    kernelH = other.kernelH;
    kernelW = other.kernelW;

    kernel = new double*[kernelH];
    for (int i = 0; i < kernelH; ++i) {
        kernel[i] = new double[kernelW];
        for (int j = 0; j < kernelW; ++j) {
            kernel[i][j] = other.kernel[i][j];
        }
    }
    strideValue = other.strideValue;
    padding = other.padding;
}

// Copy assignment operator
Convolution& Convolution::operator=(const Convolution &other) {

    if (this != &other) { // Self-assignment check
        // Free existing kernel
        if (kernel != nullptr) {
            for (int i = 0; i < kernelH; ++i) {
                delete[] kernel[i];
            }
            delete[] kernel;
        }

        // Deep copy the kernel
        if (other.kernel != nullptr) {
            kernelH = other.kernelH;
            kernelW = other.kernelW;

            kernel = new double*[kernelH];
            for (int i = 0; i < kernelH; ++i) {
                kernel[i] = new double[kernelW];
                for (int j = 0; j < kernelW; ++j) {
                    kernel[i][j] = other.kernel[i][j];
                }
            }
        } else {
            kernel = nullptr;
            kernelH = 0;
            kernelW = 0;
        }

        strideValue = other.strideValue;
        padding = other.padding;
    }
    return *this;
}

// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix& input_image) const {
    int inputHeight = input_image.get_height();
    int inputWidth = input_image.get_width();

    int outputHeight;
    int outputWidth;

    outputHeight = ((inputHeight - kernelH + 2 * padding) / strideValue) + 1;
    outputWidth = ((inputWidth - kernelW + 2 * padding) / strideValue) + 1;

    // Initialize the output feature map
    double** outputFeatureMap = new double*[outputHeight];
    for (int i = 0; i < outputHeight; ++i) {
        outputFeatureMap[i] = new double[outputWidth];
    }

    // Do the convolution
    for (int i = 0; i < outputHeight; ++i) {
        for (int j = 0; j < outputWidth; ++j) {
            outputFeatureMap[i][j] = convolutionProcess(input_image, i, j);
        }
    }

    ImageMatrix convolvedImage(const_cast<const double**>(outputFeatureMap), outputHeight, outputWidth);

    // Free the allocated memory for outputFeatureMap
    for (int i = 0; i < outputHeight; ++i) {
        delete[] outputFeatureMap[i];
    }
    delete[] outputFeatureMap;

    return convolvedImage;
}

double Convolution::convolutionProcess(const ImageMatrix& input_image, int outputRow, int outputCol) const {
    int inputHeight = input_image.get_height();
    int inputWidth = input_image.get_width();

    // Calculate the starting position for this convolution window
    int startRow = outputRow * strideValue - padding;
    int startCol = outputCol * strideValue - padding;

    // Perform the convolution for this window
    double sum = 0.0;
    for (int m = 0; m < kernelH; ++m) {
        for (int n = 0; n < kernelW; ++n) {
            int inputRow = startRow + m;
            int inputCol = startCol + n;

            if ((inputRow >= 0) && (inputRow < inputHeight) && (inputCol >= 0) && (inputCol < inputWidth)) {
                sum += input_image.get_data(inputRow, inputCol) * kernel[m][n];
            }
        }
    }
    return sum;
}
