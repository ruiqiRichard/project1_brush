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
    // if (scaleX < 1) {
    //     blur(2);
    // }
    filterScaleHelper(m_data, scaleX, m_width, m_height, true);
    m_width = new_width;

    // if (scaleY < 1) {
    //     blur(2);
    // }

    filterScaleHelper(m_data, scaleY, m_width, m_height, false);
    m_height = new_height;

    // float radiusX = scaleX >= 1 ? 1.f : 1 / scaleX;
    // float radiusY = scaleY >= 1 ? 1.f : 1 / scaleY;

    // std::vector<RGBA> buffer(new_width * m_height, RGBA{0, 0, 0, 255});

    // for (int i = 0; i < m_height; i++) {
    //     for (int j = 0; j < new_width; j++) {
    //         int index = i * new_width + j;

    //         float x = j / scaleX + (1-scaleX) / (2 * scaleX);
    //         float y = i / scaleY + (1-scaleY) / (2 * scaleY);

    //         int x_floor = scaleX >= 1 ? std::ceil(x - 1) : std::ceil(x - 1 / scaleX);
    //         int y_floor = scaleY >= 1 ? std::ceil(y - 1) : std::ceil(y - 1 / scaleY);

    //         int x_ceil = scaleX >= 1 ? std::floor(x + 1) : std::floor(x + 1 / scaleX);
    //         int y_ceil = scaleY >= 1 ? std::floor(y + 1) : std::floor(y + 1 / scaleY);


    //         double weight_sum = 0;
    //         double acc_r = 0;
    //         double acc_g = 0;
    //         double acc_b = 0;

    //         for (int kx = x_floor; kx <= x_ceil; kx++) {
    //             RGBA pixel = getPixelRepeated(m_data, m_width, m_height, kx, i);
    //             double weight = g((x-kx), radiusX);
    //             weight_sum += weight;
    //             acc_r += pixel.r * weight;
    //             acc_g += pixel.g * weight;
    //             acc_b += pixel.b * weight;
    //         }

    //         buffer[index].r += static_cast<uint8_t>(acc_r + 0.5);
    //         buffer[index].g += static_cast<uint8_t>(acc_g + 0.5);
    //         buffer[index].b += static_cast<uint8_t>(acc_b + 0.5);
            
    //         weight_sum = 0;
    //         acc_r = 0;
    //         acc_g = 0;
    //         acc_b = 0;

            // for (int ky = y_floor; ky <= y_ceil; ky++) {
            //     RGBA pixel = getPixelRepeated(buffer_X, new_width, m_height, j, ky);
            //     double weight = g((y-ky), radiusY);
            //     weight_sum += weight;
            //     acc_r += pixel.r * weight;
            //     acc_g += pixel.g * weight;
            //     acc_b += pixel.b * weight;
            // }

            // buffer[index].r += static_cast<uint8_t>(acc_r + 0.5);
            // buffer[index].g += static_cast<uint8_t>(acc_g + 0.5);
            // buffer[index].b += static_cast<uint8_t>(acc_b + 0.5);


            // // 2x2 triangle filter
            // RGBA top_left = getPixelRepeated(m_data, m_width, m_height, x_floor, y_floor);
            // RGBA top_right = getPixelRepeated(m_data, m_width, m_height, x_ceil, y_floor);
            // RGBA bottom_left = getPixelRepeated(m_data, m_width, m_height, x_floor, y_ceil);
            // RGBA bottom_right = getPixelRepeated(m_data, m_width, m_height, x_ceil, y_ceil);

            // buffer[index].r = static_cast<uint8_t>(
            //     top_left.r * (1 - delta_x) * (1 - delta_y) +
            //     top_right.r * delta_x * (1 - delta_y) +
            //     bottom_left.r * (1 - delta_x) * delta_y +
            //     bottom_right.r * delta_x * delta_y + 0.5
            // );

            // buffer[index].g = static_cast<uint8_t>(
            //     top_left.g * (1 - delta_x) * (1 - delta_y) +
            //     top_right.g * delta_x * (1 - delta_y) +
            //     bottom_left.g * (1 - delta_x) * delta_y +
            //     bottom_right.g * delta_x * delta_y + 0.5
            // );

            // buffer[index].b = static_cast<uint8_t>(
            //     top_left.b * (1 - delta_x) * (1 - delta_y) +
            //     top_right.b * delta_x * (1 - delta_y) +
            //     bottom_left.b * (1 - delta_x) * delta_y +
            //     bottom_right.b * delta_x * delta_y + 0.5
            // );
    //     }
    // }


    // m_height = new_height;
}
