#include <iostream>
#include "ImageSharpening.h"

// Default constructor
ImageSharpening::ImageSharpening() {
    kernel_height = 3;
    kernel_width = 3;

    blurring_kernel = new double*[3]{
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0},
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0},
            new double[3]{1.0/9.0, 1.0/9.0, 1.0/9.0}
    };
}

ImageSharpening::~ImageSharpening(){
    if (blurring_kernel != nullptr){
        for (int i = 0; i < kernel_height; ++i) {
            delete[] blurring_kernel[i];
        }
        delete[] blurring_kernel;
    }
}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix& input_image, double k) {
    Convolution convolved(blurring_kernel, kernel_height, kernel_width, 1, true);
    ImageMatrix blurredImage(input_image.get_height(), input_image.get_width());
    blurredImage = convolved.convolve(input_image);

    ImageMatrix sharpImage(input_image.get_height(), input_image.get_width());
    sharpImage = (input_image + (input_image - blurredImage) * k);

    for (int i = 0; i < sharpImage.get_height(); i++) {
        for (int j = 0; j < sharpImage.get_width(); j++) {
            double pixelValue = sharpImage.get_data(i, j);
            sharpImage.set_data(i, j, std::max(0.0, std::min(255.0, pixelValue)));
        }
    }
    return sharpImage;
}