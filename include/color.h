#pragma once

#include <string>
#include <stdint.h>
#include <core/SkColor.h>

namespace core
{
class Color{
public:
    unsigned char r=0,g=0,b=0,a=0;
    Color()=default;
    Color(unsigned char r,unsigned char g,unsigned char b,unsigned char a=255):r(r),g(g),b(b),a(a){}
    Color(const Color& other):r(other.r),g(other.g),b(other.b),a(other.a){}
    Color(const uint32_t& hex):r((hex>>16)&0xFF),g((hex>>8)&0xFF),b(hex&0xFF),a((hex>>24)&0xFF){}
    uint32_t toHex();
    std::string toHexStr();
    operator SkColor() const { return SkColorSetARGB(a, r, g, b); }
};
} // namespace core
