#include "filterutils.h"
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>

namespace FilterUtils {

std::uint8_t floatToUint8(float x) {
    return round(x * 255.f);
}

// getPixelRepeated(), getPixelReflected(), and getPixelWrapped() all
// have the same input arguments:
//
// data   - the image's data
// width  - the image's width
// height - the image's height
// x      - the x coordinate of the pixel you're attempting to access
// y      - the y coordinate of the pixel you're attempting to access

// Repeats the pixel on the edge of the image such that A,B,C,D looks like ...A,A,A,B,C,D,D,D...
RGBA getPixelRepeated(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? 0 : std::min(x, width  - 1);
    int newY = (y < 0) ? 0 : std::min(y, height - 1);
    return data[width * newY + newX];
}

RGBAf getPixelRepeated_f(std::vector<RGBAf> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? 0 : std::min(x, width  - 1);
    int newY = (y < 0) ? 0 : std::min(y, height - 1);
    return data[width * newY + newX];
}

// Flips the edge of the image such that A,B,C,D looks like ...C,B,A,B,C,D,C,B...
RGBA getPixelReflected(std::vector<RGBA> &data, int width, int height, int x, int y) {
    // Task 12: implement this function
    int newX = (x > width-1) ? 2 * width - x - 2 : std::abs(x);
    int newY = (y > height-1) ? 2 * height - y - 2 : std::abs(y);
    return data[width * newY + newX];
}

// Wraps the image such that A,B,C,D looks like ...C,D,A,B,C,D,A,B...
RGBA getPixelWrapped(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? x + width  : x % width;
    int newY = (y < 0) ? y + height : y % height;
    return data[width * newY + newX];
}

// Assumes the input kernel is square, and has an odd-numbered side length
void convolve2D(std::vector<RGBA> &data, int width, int height, const std::vector<float> &kernel) {
    // Task 9: initialize a vector, called `result`, to temporarily store your output image data
    std::vector<RGBA> result(width*height, {0,0,0,255});

    // Task 10: obtain the kernel's dimensions
    int dim = std::sqrt(kernel.size());
    int center = dim / 2;

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            // Task 11:
            // 1. Initialize redAcc, greenAcc, and blueAcc float variables
            // 2. Iterate over the kernel using its dimensions from task 10.
            //    - Get the value, called `weight`, of the kernel at some position.
            //      - Remember that we're doing convolution, so we must "flip" the
            //        given kernel. How can you do that through (clever) indexing?
            //    - Get the value, called `pixel`, of the corresponding pixel in the canvas.
            // 3. Accumulate `weight * pixel` for each channel in redAcc, greenAcc, and blueAcc accordingly
            float redAcc = 0, greenAcc = 0, blueAcc = 0;
            for (int kr = dim-1; kr >= 0; kr--) {
                for (int kc = dim-1; kc >= 0; kc--) {
                    size_t index = kr * dim + kc;
                    int delta_r = kr - center;
                    int delta_c = kc - center;
                    RGBA substitute = getPixelRepeated(data, width, height, c-delta_c, r-delta_r);
                    redAcc += kernel[index] * substitute.r / 255.f;
                    greenAcc += kernel[index] * substitute.g / 255.f;
                    blueAcc += kernel[index] * substitute.b / 255.f;
                }
            }

            // Task 13: Update buffer with the new RGBA pixel value created from
            //          redAcc, greenAcc, and blueAcc
            result[centerIndex].r = floatToUint8(redAcc);
            result[centerIndex].g = floatToUint8(greenAcc);
            result[centerIndex].b = floatToUint8(blueAcc);
        }
    }

    // Task 14: Copy the RGBA data from `result` (task 9) to `data`
    for (int i=0; i<result.size(); i++) {
        data[i] = result[i];
    }
}

// Assumes the input kernel is square, and has an odd-numbered side length
std::vector<RGBA> convolve2D_re(std::vector<RGBA>& data, int width, int height, const std::vector<float> &kernel) {
    // Task 9: initialize a vector, called `result`, to temporarily store your output image data
    std::vector<RGBA> result(width*height, {0,0,0,255});

    // Task 10: obtain the kernel's dimensions
    int dim = std::sqrt(kernel.size());
    int center = dim / 2;

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;

            // Task 11:
            // 1. Initialize redAcc, greenAcc, and blueAcc float variables
            // 2. Iterate over the kernel using its dimensions from task 10.
            //    - Get the value, called `weight`, of the kernel at some position.
            //      - Remember that we're doing convolution, so we must "flip" the
            //        given kernel. How can you do that through (clever) indexing?
            //    - Get the value, called `pixel`, of the corresponding pixel in the canvas.
            // 3. Accumulate `weight * pixel` for each channel in redAcc, greenAcc, and blueAcc accordingly
            float redAcc = 0, greenAcc = 0, blueAcc = 0;
            for (int kr = dim-1; kr >= 0; kr--) {
                for (int kc = dim-1; kc >= 0; kc--) {
                    size_t index = kr * dim + kc;
                    int delta_r = kr - center;
                    int delta_c = kc - center;
                    RGBA substitute = getPixelRepeated(data, width, height, c-delta_c, r-delta_r);
                    redAcc += kernel[index] * substitute.r / 255.f;
                    greenAcc += kernel[index] * substitute.g / 255.f;
                    blueAcc += kernel[index] * substitute.b / 255.f;
                }
            }

            // Task 13: Update buffer with the new RGBA pixel value created from
            //          redAcc, greenAcc, and blueAcc
            result[centerIndex].r = floatToUint8(redAcc);
            result[centerIndex].g = floatToUint8(greenAcc);
            result[centerIndex].b = floatToUint8(blueAcc);
        }
    }

    return result;
}

void convolve1D(std::vector<RGBA>& data, int width, int height, const std::vector<float>& kernel, bool horizontal) {
    std::vector<RGBA> result(width*height, {0,0,0,255});
    int dim = kernel.size();
    int center = dim / 2;
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float redAcc = 0, greenAcc = 0, blueAcc = 0;
            for (int k = 0; k < dim; k++) {
                int delta = k - center;
                RGBA substitute = horizontal ? getPixelRepeated(data, width, height, c-delta, r) : getPixelRepeated(data, width, height, c, r-delta);
                redAcc += kernel[k] * substitute.r / 255.f;
                greenAcc += kernel[k] * substitute.g / 255.f;
                blueAcc += kernel[k] * substitute.b / 255.f;
            }
            result[centerIndex].r = floatToUint8(redAcc);
            result[centerIndex].g = floatToUint8(greenAcc);
            result[centerIndex].b = floatToUint8(blueAcc);
        }
    }

    for (int i=0; i<result.size(); i++) {
        data[i] = result[i];
    }
}

std::vector<RGBAf> convolve1D_re(std::vector<RGBA>& data, int width, int height, const std::vector<float>& kernel, bool horizontal) {
    std::vector<RGBAf> result(width*height, {0,0,0,255});
    for (int i=0; i<data.size(); i++) {
        result[i] = RGBAf::fromRGBA(data[i]);
    }
    int dim = kernel.size();
    int center = dim / 2;
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float redAcc = 0, greenAcc = 0, blueAcc = 0;
            for (int k = 0; k < dim; k++) {
                int delta = k - center;
                RGBA substitute = horizontal ? getPixelRepeated(data, width, height, c-delta, r) : getPixelRepeated(data, width, height, c, r-delta);
                redAcc += kernel[k] * substitute.r;
                greenAcc += kernel[k] * substitute.g;
                blueAcc += kernel[k] * substitute.b;
            }
            result[centerIndex].r = redAcc;
            result[centerIndex].g = greenAcc;
            result[centerIndex].b = blueAcc;
        }
    }

    return result;
}

std::vector<RGBAf> convolve1D_re_f(std::vector<RGBAf>& data, int width, int height, const std::vector<float>& kernel, bool horizontal) {
    std::vector<RGBAf> result(width*height, {0,0,0,255});
    int dim = kernel.size();
    int center = dim / 2;
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            size_t centerIndex = r * width + c;
            float redAcc = 0, greenAcc = 0, blueAcc = 0;
            for (int k = 0; k < dim; k++) {
                int delta = k - center;
                RGBAf substitute = horizontal ? getPixelRepeated_f(data, width, height, c-delta, r) : getPixelRepeated_f(data, width, height, c, r-delta);
                redAcc += kernel[k] * substitute.r;
                greenAcc += kernel[k] * substitute.g;
                blueAcc += kernel[k] * substitute.b;
            }
            result[centerIndex].r = redAcc;
            result[centerIndex].g = greenAcc;
            result[centerIndex].b = blueAcc;
        }
    }

    return result;
}

int get_x(int index, int radius) {
    return index % (2*radius + 1);
}

int get_y(int index, int radius) {
    return index / (2*radius + 1);
}

float getDistance(int x, int y, int radius) {
    return std::sqrt(std::pow(x-radius,2) + std::pow(y-radius,2));
}

}

