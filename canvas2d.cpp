#include "canvas2d.h"
#include <QPainter>
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include "settings.h"
#include "brushmask.h"
#include <queue>

/**
 * @brief Initializes new 500x500 canvas
 */
void Canvas2D::init() {
    setMouseTracking(true);
    m_width = 500;
    m_height = 500;
    clearCanvas();
}

/**
 * @brief Canvas2D::clearCanvas sets all canvas pixels to blank white
 */
void Canvas2D::clearCanvas() {
    m_data.assign(m_width * m_height, RGBA{255, 255, 255, 255});
    settings.imagePath = "";
    displayImage();
}

/**
 * @brief Stores the image specified from the input file in this class's
 * `std::vector<RGBA> m_image`.
 * Also saves the image width and height to canvas width and height respectively.
 * @param file: file path to an image
 * @return True if successfully loads image, False otherwise.
 */
bool Canvas2D::loadImageFromFile(const QString &file) {
    QImage myImage;
    if (!myImage.load(file)) {
        std::cout<<"Failed to load in image"<<std::endl;
        return false;
    }
    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
    m_width = myImage.width();
    m_height = myImage.height();
    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

    m_data.clear();
    m_data.reserve(m_width * m_height);
    for (int i = 0; i < arr.size() / 4; i++){
        m_data.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
    }
    displayImage();
    return true;
}

/**
 * @brief Saves the current canvas image to the specified file path.
 * @param file: file path to save image to
 * @return True if successfully saves image, False otherwise.
 */
bool Canvas2D::saveImageToFile(const QString &file) {
    QImage myImage = QImage(m_width, m_height, QImage::Format_RGBX8888);
    for (int i = 0; i < m_data.size(); i++){
        myImage.setPixelColor(i % m_width, i / m_width, QColor(m_data[i].r, m_data[i].g, m_data[i].b, m_data[i].a));
    }
    if (!myImage.save(file)) {
        std::cout<<"Failed to save image"<<std::endl;
        return false;
    }
    return true;
}


/**
 * @brief Get Canvas2D's image data and display this to the GUI
 */
void Canvas2D::displayImage() {
    QByteArray img(reinterpret_cast<const char *>(m_data.data()), 4 * m_data.size());
    QImage now = QImage((const uchar*)img.data(), m_width, m_height, QImage::Format_RGBX8888);
    setPixmap(QPixmap::fromImage(now));
    setFixedSize(m_width, m_height);
    update();
}

/**
 * @brief Canvas2D::resize resizes canvas to new width and height
 * @param w
 * @param h
 */
void Canvas2D::resize(int w, int h) {
    m_width = w;
    m_height = h;
    m_data.resize(w * h);
    displayImage();
}

/**
 * @brief Called when the filter button is pressed in the UI
 */
void Canvas2D::filterImage() {
    // Filter TODO: apply the currently selected filter to the loaded image
}

/**
 * @brief Called when any of the parameters in the UI are modified.
 */
void Canvas2D::settingsChanged() {
    // this saves your UI settings locally to load next time you run the program
    settings.saveSettings();

    // TODO: fill in what you need to do when brush or filter parameters change
    settings.loadSettingsOrDefaults();
    brushColor.r = settings.brushColor.r;
    brushColor.g = settings.brushColor.g;
    brushColor.b = settings.brushColor.b;
    brushColor.a = settings.brushColor.a;
    mask.changeRadius(settings.brushRadius);
    mask.changeType(settings.brushType);
}

/**
 * @brief These functions are called when the mouse is clicked and dragged on the canvas
 */
void Canvas2D::mouseDown(int x, int y) {
    m_isDown = true;
    if (x > -1 && x < m_width && y > -1 && y < m_height) {
        if (settings.fixAlphaBlending) {
            if (settings.brushRadius != 0) {
                for (auto i : m_data) {
                    RGBA maxColor = {(uint8_t)(1 * (brushColor.a / 255.0) * brushColor.r + i.r * (1 - 1 * (brushColor.a / 255.0)) + 0.5),
                                    (uint8_t)(1 * (brushColor.a / 255.0) * brushColor.g + i.g * (1 - 1 * (brushColor.a / 255.0)) + 0.5),
                                    (uint8_t)(1 * (brushColor.a / 255.0) * brushColor.b + i.b * (1 - 1 * (brushColor.a / 255.0)) + 0.5),
                                    (uint8_t)(brushColor.a)};
                    maxBlending.push_back(maxColor);
                }
            }

        }
        if (settings.brushType == BRUSH_FILL) {
            int index = y * m_width + x;
            RGBA fillin{(uint8_t)((brushColor.a / 255.0) * brushColor.r + m_data[index].r * (1-1 * (brushColor.a / 255.0)) + 0.5),
                               (uint8_t)((brushColor.a / 255.0) * brushColor.g + m_data[index].g * (1-1 * (brushColor.a / 255.0)) + 0.5),
                               (uint8_t)((brushColor.a / 255.0) * brushColor.b + m_data[index].b * (1-1 * (brushColor.a / 255.0)) + 0.5),
                               (uint8_t)(brushColor.a)};
            fill(m_data[index], x, y, fillin);

        }
        else if (settings.brushType == BRUSH_SMUDGE){
            pickUpPrevColors(x,y);
        }
        else {
            if (settings.brushRadius != 0) {
                brush(x,y);
            }
        }
    }
    displayImage();
}

void Canvas2D::mouseDragged(int x, int y) {
    if (x > -1 && x < m_width && y > -1 && y < m_height && m_isDown) {
        if (settings.brushType == BRUSH_SMUDGE){
            smudge(x,y);
        }
        else {
            if (settings.brushRadius != 0) {
                brush(x,y);
            }

        }


    }
    displayImage();

}

void Canvas2D::mouseUp(int x, int y) {
    prevColors.clear();
    maxBlending.clear();
    m_isDown = false;
}

void Canvas2D::brush(int x, int y) {
    brushmask mask(settings.brushRadius,settings.brushType, settings.brushDensity);
    for (int i=0; i < mask.m_opacity.size(); i++) {
        int delta_x = mask.get_x(i) - mask.getRadius();
        int delta_y = mask.get_y(i) - mask.getRadius();
        int index = (delta_x + x) + (delta_y + y) * m_width;

        // fix alpha blending
        if (settings.fixAlphaBlending) {
            if (delta_x + x > -1 && delta_x + x < m_width && delta_y + y > -1 && delta_y + y < m_height) {
                m_data[index] = RGBA{std::max((uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.r + m_data[index].r * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5), maxBlending[index].r),
                                     std::max((uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.g + m_data[index].g * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5), maxBlending[index].g),
                                     std::max((uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.b + m_data[index].b * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5), maxBlending[index].b),
                                     (uint8_t)(brushColor.a)};
            }
        }
        else {
            if (delta_x + x > -1 && delta_x + x < m_width && delta_y + y > -1 && delta_y + y < m_height) {
                m_data[index] = RGBA{(uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.r + m_data[index].r * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5),
                                     (uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.g + m_data[index].g * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5),
                                     (uint8_t)(mask.m_opacity[i] * (brushColor.a / 255.0) * brushColor.b + m_data[index].b * (1-mask.m_opacity[i] * (brushColor.a / 255.0)) + 0.5),
                                     (uint8_t)(brushColor.a)};
            }
        }



    }
}

void Canvas2D::smudge(int x, int y) {
    brushmask mask(settings.brushRadius,settings.brushType, settings.brushDensity);
    for (int i=0; i < mask.m_opacity.size(); i++) {
        int delta_x = mask.get_x(i) - mask.getRadius();
        int delta_y = mask.get_y(i) - mask.getRadius();
        int index = (delta_x + x) + (delta_y + y) * m_width;

        if (delta_x + x > -1 && delta_x + x < m_width && delta_y + y > -1 && delta_y + y < m_height) {
            m_data[index] = RGBA{(uint8_t)((mask.m_opacity[i] * (prevColors[i].a / 255.0) * prevColors[i].r) + (1-mask.m_opacity[i] * (prevColors[i].a / 255.0)) * m_data[index].r + 0.5),
                                 (uint8_t)((mask.m_opacity[i] * (prevColors[i].a / 255.0) * prevColors[i].g) + (1-mask.m_opacity[i] * (prevColors[i].a / 255.0)) * m_data[index].g + 0.5),
                                 (uint8_t)((mask.m_opacity[i] * (prevColors[i].a / 255.0) * prevColors[i].b) + (1-mask.m_opacity[i] * (prevColors[i].a / 255.0)) * m_data[index].b + 0.5),
                                 (uint8_t)(prevColors[i].a)};
        }

    }
    prevColors.clear();
    pickUpPrevColors(x,y);

}

void Canvas2D::pickUpPrevColors(int x, int y) {
    for (int i=0; i < std::pow(2*settings.brushRadius+1,2); i++) {
        int delta_x = mask.get_x(i) - settings.brushRadius;
        int delta_y = mask.get_y(i) - settings.brushRadius;
        if (delta_x + x > -1 && delta_x + x < m_width && delta_y + y > -1 && delta_y + y < m_height) {
            int index = (delta_x + x) + (delta_y + y) * m_width;
            prevColors.push_back(m_data[index]);
        }
        else {
            prevColors.push_back(RGBA{0,0,0,0});
        }
    }
}

void Canvas2D::fill(RGBA currColor, int x, int y, RGBA color) {
    int delta_x[4] = {1, 0, 0, -1};
    int delta_y[4] = {0, 1, -1, 0};

    if (currColor.equals(color)) {
        return;
    }
    else {
        std::queue<std::pair<int, int>> q;
        q.emplace(x,y);
        int index = y * m_width + x;
        m_data[index] = color;
        while (!q.empty()) {
            int x = q.front().first, y = q.front().second;
            q.pop();
            for (int i = 0; i < 4; i++) {
                int mx = x + delta_x[i], my = y + delta_y[i];
                index = my * m_width + mx;
                if (mx >= 0 && mx < m_width && my >= 0 && my < m_height && m_data[index].equals(currColor)) {
                    q.emplace(mx, my);
                    m_data[index] = color;
                }
            }
        }
    }


}





