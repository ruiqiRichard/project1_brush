#include "canvas2d.h"
#include "filterutils.h"
#include "settings.h"
#include "algorithm"
#include "iostream"

void Canvas2D::edgeDetect() {
    std::vector<float> sobel_kernel_1 = {1, 2, 1};
    std::vector<float> sobel_kernel_2 = {-1, 0, 1};

    std::vector<RGBAf> h_x = FilterUtils::convolve1D_re(m_data, m_width, m_height, sobel_kernel_2, true);
    std::vector<RGBAf> G_x = FilterUtils::convolve1D_re_f(h_x, m_width, m_height, sobel_kernel_1, false);

    std::vector<RGBAf> h_y = FilterUtils::convolve1D_re(m_data, m_width, m_height, sobel_kernel_1, true);
    std::vector<RGBAf> G_y = FilterUtils::convolve1D_re_f(h_y, m_width, m_height, sobel_kernel_2, false);

    for (int i = 0; i < m_data.size(); i++) {
        float r = settings.edgeDetectSensitivity * settings.edgeDetectSensitivity * std::sqrt(G_x[i].r * G_x[i].r + G_y[i].r * G_y[i].r);
        float g = settings.edgeDetectSensitivity * settings.edgeDetectSensitivity * std::sqrt(G_x[i].g * G_x[i].g + G_y[i].g * G_y[i].g);
        float b = settings.edgeDetectSensitivity * settings.edgeDetectSensitivity * std::sqrt(G_x[i].b * G_x[i].b + G_y[i].b * G_y[i].b);
        r = std::clamp(r, 0.f, 255.f) / 255.f;
        g = std::clamp(g, 0.f, 255.f) / 255.f;
        b = std::clamp(b, 0.f, 255.f) / 255.f;
        m_data[i] = RGBA{FilterUtils::floatToUint8(r), FilterUtils::floatToUint8(g), FilterUtils::floatToUint8(b), 255};
    }
}
