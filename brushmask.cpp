#include "brushmask.h"
#include "cmath"
#include "settings.h"
#include "iostream"

brushmask::brushmask() {
    radius = settings.brushRadius;
    brushtype = settings.brushType;
    // std::cout << radius << std::endl;
}

brushmask::brushmask(int r, int type, int density) {
    radius = r;
    brushtype = type;
    spray_density = density;
    float A = 1.0 / (radius*radius);
    float B = -2.0 / radius;
    switch (brushtype) {
    case BRUSH_CONSTANT:
        for (int i=0; i < std::pow(2*r+1,2); i++) {
            int x = get_x(i);
            int y = get_y(i);
            if (getDistance(x,y) <= r) {
                m_opacity.push_back(1.0);
            }
            else {
                m_opacity.push_back(0.0);
            }
        }
        break;

    case BRUSH_LINEAR:
        for (int i=0; i < std::pow(2*r+1,2); i++) {
            int x = get_x(i);
            int y = get_y(i);
            m_opacity.push_back(std::max(1.0-getDistance(x,y)/radius,0.0));
        }
        break;

    case BRUSH_QUADRATIC:

        for (int i=0; i < std::pow(2*r+1,2); i++) {
            int x = get_x(i);
            int y = get_y(i);
            if (getDistance(x,y) > radius){
                m_opacity.push_back(0.0);
            }
            else
            {
                m_opacity.push_back(std::max(A * std::pow(getDistance(x,y),2) + B * getDistance(x,y) + 1.0, 0.0));
            }
        }
        break;

    case BRUSH_SMUDGE:
        for (int i=0; i < std::pow(2*r+1,2); i++) {
            int x = get_x(i);
            int y = get_y(i);
            if (getDistance(x,y) > radius){
                m_opacity.push_back(0.0);
            }
            else
            {
                m_opacity.push_back(std::max(A * std::pow(getDistance(x,y),2) + B * getDistance(x,y) + 1.0, 0.0));
            }
        }
        break;

    case BRUSH_SPRAY:
        for (int i=0; i < std::pow(2*r+1,2); i++) {
            int x = get_x(i);
            int y = get_y(i);
            if (getDistance(x,y) <= r & (std::rand() % 100) < spray_density) {
                m_opacity.push_back(1.0);
            }
            else {
                m_opacity.push_back(0.0);
            }
        }
        break;
    }




}

int brushmask::get_x(int index) {
    return index % (2*radius + 1);
}

int brushmask::get_y(int index) {
    return index / (2*radius + 1);
}

float brushmask::getDistance(int x, int y) {
    return std::sqrt(std::pow(x-radius,2) + std::pow(y-radius,2));
}

int brushmask::getRadius() {
    return radius;
}

void brushmask::changeRadius(int rad) {
    radius = rad;
}

void brushmask::changeType(int type) {
    brushtype = type;
}
