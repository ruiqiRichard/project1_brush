#include "rgba.h"
#include <cstdint>
#include <vector>

namespace FilterUtils {

std::uint8_t floatToUint8(float x);
RGBA getPixelRepeated(std::vector<RGBA> &data, int width, int height, int x, int y);

void convolve2D(std::vector<RGBA>& data,
                int width,
                int height,
                const std::vector<float>& kernel);

void convolve1D(std::vector<RGBA>& data,
                int width,
                int height,
                const std::vector<float>& kernel,
                bool horizontal);

int get_x(int index, int radius);
int get_y(int index, int radius);
float getDistance(int x, int y, int radius);
std::vector<RGBA> convolve2D_re(std::vector<RGBA>& data, int width, int height, const std::vector<float> &kernel);
std::vector<RGBAf> convolve1D_re(std::vector<RGBA>& data, int width, int height, const std::vector<float>& kernel, bool horizontal);
std::vector<RGBAf> convolve1D_re_f(std::vector<RGBAf>& data, int width, int height, const std::vector<float>& kernel, bool horizontal);
}
