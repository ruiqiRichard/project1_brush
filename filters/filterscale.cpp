#include "canvas2d.h"
#include "filterutils.h"
#include "iostream"

RGBA getPixelRepeated(std::vector<RGBA> &data, int width, int height, int x, int y) {
    int newX = (x < 0) ? 0 : std::min(x, width  - 1);
    int newY = (y < 0) ? 0 : std::min(y, height - 1);
    return data[width * newY + newX];
}

double g(double x, double a) {
    double radius;
    if (a < 1) {
        radius = 1.0 / a;
    } else {
        radius = 1.0;
    }
    if ( x < -radius || x > radius) {
        return 0;
    }
    else {
        return (1 - std::fabs(x) / radius) / radius;
    }
}

void filterScaleHelper(std::vector<RGBA>& m_data, float factor, int width, int height, bool horizontal) {
    int new_width = horizontal ? std::round(width * factor) : width;
    int new_height = horizontal ? height : std::round(height * factor);
    float radius = factor >= 1 ? 1.f : 1 / factor;
    std::vector<RGBA> buffer(new_width * new_height, RGBA{0, 0, 0, 255});

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            int index = i * new_width + j;

            float c = horizontal ? (j / factor) : (i / factor);
            c += (1-factor) / (2 * factor);

            int floor = factor >= 1 ? std::ceil(c - 1) : std::ceil(c - 1 / factor);
            int ceil = factor >= 1 ? std::floor(c + 1) : std::floor(c + 1 / factor);


            double weight_sum = 0;
            double acc_r = 0;
            double acc_g = 0;
            double acc_b = 0;

            for (int k = floor; k <= ceil; k++) {
                RGBA pixel = horizontal ? getPixelRepeated(m_data, width, height, k, i) : getPixelRepeated(m_data, width, height, j, k);
                double weight = g((c-k), radius);
                weight_sum += weight;
                acc_r += pixel.r * weight;
                acc_g += pixel.g * weight;
                acc_b += pixel.b * weight;
            }

            buffer[index].r += static_cast<uint8_t>(acc_r / weight_sum + 0.5);
            buffer[index].g += static_cast<uint8_t>(acc_g / weight_sum + 0.5);
            buffer[index].b += static_cast<uint8_t>(acc_b / weight_sum + 0.5);
        }
    }

    m_data.resize(new_width * new_height);
    for (int i = 0; i < buffer.size(); i++) {
        m_data[i] = buffer[i];
    }
}

void Canvas2D::filterScale(float scaleX, float scaleY) {
    int new_width = std::round(m_width * scaleX);
    int new_height = std::round(m_height * scaleY);

    filterScaleHelper(m_data, scaleX, m_width, m_height, true);
    m_width = new_width;

    filterScaleHelper(m_data, scaleY, m_width, m_height, false);
    m_height = new_height;

}