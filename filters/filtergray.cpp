#include <cstdint>
#include "canvas2d.h"
#include "filterutils.h"

std::uint8_t rgbaToGray(const RGBA &pixel) {
    // Task 3: Calculate and return the gray value of the
    //         pixel using its RGB components.
    // Note: We are returning an std::uint8_t, because it is 1 byte
    //       and can store values from 0-255!
    float avg_val = 0.299*pixel.r + 0.587*pixel.g + 0.114*pixel.b + 0.5;

    return (std::uint8_t)avg_val;
}

void Canvas2D::filterGray() {
    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            size_t currentIndex = m_width * row + col;
            RGBA &currentPixel = m_data[currentIndex];

            // Task 2: call rgbaToGray()
            std::uint8_t gray_val = rgbaToGray(currentPixel);

            // Task 4: Update currentPixel's color
            currentPixel.r = gray_val;
            currentPixel.g = gray_val;
            currentPixel.b = gray_val;
        }
    }
}
