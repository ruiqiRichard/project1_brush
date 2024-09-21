#pragma once

#include <cstdint>

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
