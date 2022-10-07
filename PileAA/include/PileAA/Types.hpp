#pragma once

#include <cstdint>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

namespace paa {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
// using u128 = std::uint128_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
// using i128 = std::int128_t;
using f32 = float;
using f64 = double;

using usize = std::size_t;
// using isize = std::ptrdiff_t;

using Keyboard = sf::Keyboard;
using Joystick = sf::Joystick;
using Mouse = sf::Mouse;
using Event = sf::Event;
using Vector2f = sf::Vector2f;
using Vector2i = sf::Vector2i;
using Vector2u = sf::Vector2u;
using Vector3f = sf::Vector3f;
using Vector3i = sf::Vector3i;
using IntRect = sf::IntRect;
using FloatRect = sf::FloatRect;
using Color = sf::Color;
using Font = sf::Font;
using Texture = sf::Texture;
using Text = sf::Text;
using BaseSprite = sf::Sprite;
using Sound = sf::Sound;
using SoundBuffer = sf::SoundBuffer;
using Music = sf::Music;
using Image = sf::Image;
using Drawable = sf::Drawable;
using Transformable = sf::Transformable;
using RenderWindow = sf::RenderWindow;
using Window = sf::RenderWindow;
using Vertex = sf::Vertex;
using VertexArray = sf::VertexArray;
using RenderStates = sf::RenderStates;
using RenderTarget = sf::RenderTarget;
using Transform = sf::Transform;
using VideoMode = sf::VideoMode;
using Clock = sf::Clock;

template <typename T> using Vector2 = sf::Vector2<T>;
template <typename T> using Vector3 = sf::Vector3<T>;
template <typename T> using Rect = sf::Rect<T>;

} // namespace paa