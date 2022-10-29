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
    FrameBuffer(const unsigned int& width, const unsigned int& height)
        : _width(width)
        , _height(height)
        , _data(new paa::u8[width * height * 4])
    {
    }

    ~FrameBuffer() { delete[] _data; }

    void put(const unsigned int& x, const unsigned int& y, const Color& color)
    {
        if (!(x < _width && y < _height))
            return;

        const paa::u8 rgba[4] = { static_cast<paa::u8>(color.r),
            static_cast<paa::u8>(color.g), static_cast<paa::u8>(color.b),
            static_cast<paa::u8>(color.a) };

        std::memcpy(_data + get_position(x, y), rgba, 4);
    }

    void rect(const unsigned int& x, const unsigned int& y,
        const unsigned int& w, const unsigned int& h, const Color& color)
    {
        const auto xmax = x + w;
        const auto ymax = y + h;

        for (auto i = x; i < xmax && i < _height; ++i) {
            for (auto j = y; j < ymax && j < _width; ++j) {
                put(i, j, color);
            }
        }
    }

    void square(const unsigned int& x, const unsigned int& y,
        const unsigned int& size, const Color& color)
    {
        rect(x, y, size, size, color);
    }

    void circle(const unsigned int& x, const unsigned int& y,
        const unsigned int& radius, const Color& color)
    {
        const auto xmax = x + radius;
        const auto ymax = y + radius;

        for (auto i = x - radius; i < xmax && i < _height; ++i) {
            for (auto j = y - radius; j < ymax && j < _width; ++j) {
                if ((i - x) * (i - x) + (j - y) * (j - y) <= radius * radius)
                    put(i, j, color);
            }
        }
    }

    void clear(const Color& color = Color::Black)
    {
        for (auto i = 0; i < _width * _height; ++i) {
            put(i % _width, i / _width, color);
        }
    }

    paa::u8* pixels() const { return _data; }

    Texture toTexture() const
    {
        Texture texture;

        texture.create(_width, _height);
        texture.update(_data);
        return texture;
    }

    Image toImage() const
    {
        Image image;
        image.create(_width, _height, _data);
        return image;
    }

    static Texture generateDefaultTexture(
        const unsigned int& width = 100, const unsigned int& height = 100)
    {
        FrameBuffer buffer(width, height);

        buffer.rect(0, 0, width / 2, height / 2, Color::Black);
        buffer.rect(width / 2, height / 2, width / 2, height / 2, Color::Black);
        buffer.rect(0, height / 2, width / 2, height / 2, Color::Magenta);
        buffer.rect(width / 2, 0, width / 2, height / 2, Color::Magenta);

        buffer.rect(0, 0, width / 10, height, Color::White);
        buffer.rect(0, 0, width, height / 10, Color::White);
        buffer.rect(width - width / 10, 0, width / 10, height, Color::White);
        buffer.rect(0, height - height / 10, width, height / 10, Color::White);
        return buffer.toTexture();
    }

    static Image generateDefaultImage(
        const unsigned int& width = 100, const unsigned int& height = 100)
    {
        return generateDefaultTexture(width, height).copyToImage();
    }
};
}