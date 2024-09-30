#include "canvas2d.h"
#include "filterutils.h"
#include "iostream"

std::vector<float> createBlurKernel(int radius) {
    // Gaussian kernel
    std::vector<float> kernel;
    float std = radius / 3.0f;
    for (int i = 0; i < 2 * radius + 1; i++) {
        float x = i - radius;
        kernel.push_back(std::exp(-x * x / (2 * std * std)) / std::sqrt(2 * M_PI * std * std));
    }
    // normalize
    float sum = 0;
    for (int i = 0; i < kernel.size(); i++) {
        sum += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= sum;
    }
    return kernel;
}

void Canvas2D::blur(int radius) {
    std::vector<float> kernel = createBlurKernel(radius);
    FilterUtils::convolve1D(m_data, m_width, m_height, kernel, true); // filter along x
    FilterUtils::convolve1D(m_data, m_width, m_height, kernel, false); // filter along y
}
