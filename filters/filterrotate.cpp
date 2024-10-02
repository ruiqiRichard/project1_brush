#include "canvas2d.h"
#include "filterutils.h"

void Canvas2D::rotate(float angle) {
    
    float angle_rad = angle * M_PI / 180;
    float center_x = m_width / 2;
    float center_y = m_height / 2;

    int new_width = fabs(m_width * cos(angle_rad)) + fabs(m_height * sin(angle_rad)) + 0.5;
    int new_height = fabs(m_width * sin(angle_rad)) + fabs(m_height * cos(angle_rad)) + 0.5;
    std::vector<RGBA> buffer(new_width * new_height, RGBA{0, 0, 0, 255});
    int new_center_x = new_width / 2;
    int new_center_y = new_height / 2;

    for (int i = 0; i < new_height; i++) {
        for (int j = 0; j < new_width; j++) {
            float x = (j - new_center_x) * cos(angle_rad) - (i - new_center_y) * sin(angle_rad) + center_x + 0.5;
            float y = (j - new_center_x) * sin(angle_rad) + (i - new_center_y) * cos(angle_rad) + center_y + 0.5;

            if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
                buffer[i * new_width + j] = FilterUtils::getPixelRepeated(m_data, m_width, m_height, x, y);
            }
        }
    }

    m_data.resize(new_width * new_height);
    for (int i = 0; i < buffer.size(); i++) {
        m_data[i] = buffer[i];
    }
    m_width = new_width;
    m_height = new_height;
}
