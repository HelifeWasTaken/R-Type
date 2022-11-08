#include "Types.hpp"
#include <cstring>

namespace paa {

class FrameBuffer {
private:
    paa::u8* _data;
    paa::usize _width, _height;

    paa::usize get_position(paa::usize x, paa::usize y) const
    {
        return (y * _width + x) * 4;
    }

public:
    FrameBuffer(const unsigned int& width, const unsigned int& height);

    ~FrameBuffer();

    void put(const unsigned int& x, const unsigned int& y, const Color& color);

    void rect(const unsigned int& x, const unsigned int& y,
        const unsigned int& w, const unsigned int& h, const Color& color);

    void square(const unsigned int& x, const unsigned int& y,
        const unsigned int& size, const Color& color);

    void circle(const unsigned int& x, const unsigned int& y,
        const unsigned int& radius, const Color& color);

    void clear(const Color& color = Color::Black);

    paa::u8* pixels() const;

    Texture toTexture() const;

    Image toImage() const;

    static Texture generateDefaultTexture(
        const unsigned int& width = 100, const unsigned int& height = 100);

    static Image generateDefaultImage(
        const unsigned int& width = 100, const unsigned int& height = 100);
};
}