#ifndef BRUSHMASK_H
#define BRUSHMASK_H

#include <vector>

class brushmask
{
public:
    brushmask();
    brushmask(int r, int type, int density);
    std::vector<float> m_opacity;
    int get_x(int index);
    int get_y(int index);
    float getDistance(int x, int y);
    int getRadius();
    void changeRadius(int rad);
    void changeType(int type);

private:
    int radius;
    int brushtype;
    int spray_density;
};

#endif // BRUSHMASK_H
