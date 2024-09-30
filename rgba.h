#pragma once

#include <cstdint>
#include <algorithm>

struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a = 255;

    bool equals (RGBA in){
        if (in.r == r && in.g==g && in.b==b && in.a==a) {
            return true;
        }
        else {
            return false;
        }
    }
};

struct RGBAf {
    // Task 7: Edit the field types so that they can represent intensity
    // values larger than 255!
    float r;
    float g;
    float b;
    float a = 255;


    RGBA toRGBA() {
        RGBA result;

        // Task 8: Complete the toRGBA() function to clamp your float values and return a RGBA struct.
        result.r = (std::uint8_t) std::min(r + 0.5, 255.0);
        result.g = (std::uint8_t) std::min(g + 0.5, 255.0);
        result.b = (std::uint8_t) std::min(b + 0.5, 255.0);
        result.a = (std::uint8_t) std::min(a + 0.5, 255.0);
        return result;
    }

    static RGBAf fromRGBA(RGBA pixel) {
        // TODO: Uncomment after Task 7!
        return RGBAf{ float(pixel.r), float(pixel.g), float(pixel.b), float(pixel.a) };
    }

};
