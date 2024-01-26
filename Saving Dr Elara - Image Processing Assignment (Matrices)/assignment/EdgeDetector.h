    #ifndef EDGE_DETECTOR_H
    #define EDGE_DETECTOR_H

    #include "ImageMatrix.h"
    #include "Convolution.h"
    #include <vector>

    class EdgeDetector {
        public:
            EdgeDetector();
            ~EdgeDetector();
            
            std::vector<std::pair<int, int>> detectEdges(const ImageMatrix& input_image);

        private:
        double **Gx;
        double **Gy;

    };

    #endif // EDGE_DETECTOR_H


