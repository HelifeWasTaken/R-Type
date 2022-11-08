#include "PileAA/FrameBuffer.hpp"

namespace paa {

    FrameBuffer::FrameBuffer(const unsigned int& width, const unsigned int& height)
        : _width(width)
        , _height(height)
        , _data(new paa::u8[width * height * 4])
    {
    }

    FrameBuffer::~FrameBuffer() { delete[] _data; }

    void FrameBuffer::put(const unsigned int& x, const unsigned int& y, const Color& color)
    {
        if (!(x < _width && y < _height))
            return;

        const paa::u8 rgba[4] = { static_cast<paa::u8>(color.r),
            static_cast<paa::u8>(color.g), static_cast<paa::u8>(color.b),
            static_cast<paa::u8>(color.a) };

        std::memcpy(_data + get_position(x, y), rgba, 4);
    }

    void FrameBuffer::rect(const unsigned int& x, const unsigned int& y,
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

    void FrameBuffer::square(const unsigned int& x, const unsigned int& y,
        const unsigned int& size, const Color& color)
    {
        rect(x, y, size, size, color);
    }

    void FrameBuffer::circle(const unsigned int& x, const unsigned int& y,
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

    void FrameBuffer::clear(const Color& color)
    {
        for (auto i = 0; i < _width * _height; ++i) {
            put(i % _width, i / _width, color);
        }
    }

    paa::u8* FrameBuffer::pixels() const { return _data; }

    Texture FrameBuffer::toTexture() const
    {
        Texture texture;

        texture.create(_width, _height);
        texture.update(_data);
        return texture;
    }

    Image FrameBuffer::toImage() const
    {
        Image image;
        image.create(_width, _height, _data);
        return image;
    }

    Texture FrameBuffer::generateDefaultTexture(const unsigned int& width, const unsigned int& height)
    {
        FrameBuffer buffer(width, height);

        paa::IntRect midTop(width / 2 - width / 4, 0, width / 3, height / 3);
        paa::IntRect midLeft(0, height / 2 - height / 4, width / 3, height / 3);
        paa::IntRect midRight(width - width / 4, height / 3, width / 3, height / 3);
        paa::IntRect midBottom(width / 2 - width / 4, height - height / 4, width / 3, height / 3);

        buffer.rect(0, 0, width, height, Color::White);

        buffer.rect(midTop.left, midTop.top, midTop.width, midTop.height, Color::Red);
        buffer.rect(midLeft.left, midLeft.top, midLeft.width, midLeft.height, Color::Green);
        buffer.rect(midRight.left, midRight.top, midRight.width, midRight.height, Color::Blue);
        buffer.rect(midBottom.left, midBottom.top, midBottom.width, midBottom.height, Color::Yellow);

        return buffer.toTexture();
    }

    Image FrameBuffer::generateDefaultImage(const unsigned int& width, const unsigned int& height)
    {
        return generateDefaultTexture(width, height).copyToImage();
    }
}