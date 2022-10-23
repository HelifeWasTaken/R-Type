#pragma once

#include <concepts>
#include <array>
#include <ostream>
#include <string>
#include <type_traits>
#include <cmath>

namespace obscur {

    constexpr double PI = 3.14159265358979323846;

    template<typename T>
    concept Number = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template<Number T, size_t N>
    class Vector;

    template<Number T>
    class Vector2;
    template<Number T>
    class Vector3;

    template<Number T, size_t N>
    class Vector {
        std::array<T, N> _values;

        public:
            Vector() = default;

            template<Number... Args>
            Vector(const Args&... value) {
                _values = {static_cast<T>(value)...};
            }
            virtual ~Vector() = default;

            const T &get(const size_t position) const {
                return _values[position];
            }

            void set(const size_t position, const T &value) {
                _values[position] = value;
            }

            const std::array<T, N> &unwrap() const {
                return _values;
            }

            template<Number T2>
            void operator+=(const Vector<T2, N> &other) {
                for (size_t i = 0; i < N; ++i) {
                    _values[i] += other._values[i];
                }
            }

            const T &operator[](const size_t &position) const {
                return _values[position];
            }

            operator Vector2<T>() {
                return Vector2<T>(_values[0], _values[1]);
            }

            template<Number T2>
            bool operator==(const Vector<T2, N> &other) const {
                for (size_t i = 0; i < N; ++i) {
                    if (_values[i] != other._values[i]) {
                        return false;
                    }
                }

                return true;
            }
    };

    template<Number T>
    class Vector2 : public Vector<T, 2> {

        //using Vector<T, 2>::Vector;
        public:
            Vector2() = default;
            Vector2(const T &x, const T &y): Vector<T, 2>(x, y) {}
            virtual ~Vector2() override = default;

            const T &x() const { return this->get(0); }
            const T &y() const { return this->get(1); }

            void x(const T &x) { this->set(0, x); }
            void y(const T &y) { this->set(1, y); }

            template<Number T2>
            operator Vector2<T2>() const {
                return Vector2<T2>(this->x(), this->y());
            }

            operator sf::Vector2f() const {
                return sf::Vector2f(this->x(), this->y());
            }

            operator sf::Vector2i() const {
                return sf::Vector2i(this->x(), this->y());
            }

            operator sf::Vector2u() const {
                return sf::Vector2u(this->x(), this->y());
            }
    };

    template<Number T>
    class Vector3 : public Vector<T, 3> {

        using Vector<T, 2>::Vector;
        public:
            Vector3() = default;
            Vector3(const T &x, const T &y, const T &z): Vector<T, 3>(x, y, z) {}
            virtual ~Vector3() override = default;

            const T &x() const { return this->get(0); }
            const T &y() const { return this->get(1); }
            const T &z() const { return this->get(2); }

            void x(const T &x) { this->set(0, x); }
            void y(const T &y) { this->set(1, y); }
            void z(const T &z) { this->set(2, z); }

            operator sf::Vector3f() const {
                return sf::Vector3f(this->x(), this->y(), this->z());
            }

            operator sf::Vector3i() const {
                return sf::Vector3i(this->x(), this->y(), this->z());
            }
    };

    template<Number T, size_t N>
    std::ostream &operator<<(std::ostream &os, const Vector<T, N> &vector) {
        std::string str = std::string("Vector<") + std::to_string(N) + std::string(">[");
        for (size_t i = 0; i < N; ++i) {
            str += std::to_string(vector.get(i));
            if (i < (N - 1))
                str += "x";
        }
        str += "]";
        os << str;
        return os;
    }

    template<Number T, size_t N>
    Vector<T, N> operator-(const Vector<T, N> &vector) {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result.set(i, -vector.get(i));
        }
        return result;
    }

    template<Number T, Number T2, size_t N>
    Vector<T, N> operator+(const Vector<T, N> &vector, const Vector<T2, N> &other) {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result.set(i, vector.get(i) + other.get(i));
        }
        return result;
    }

    template<Number T, Number T2, size_t N>
    Vector<T, N> operator-(const Vector<T, N> &vector, const Vector<T2, N> &other) {
        Vector<T, N> result;
        for (size_t i = 0; i < N; ++i) {
            result.set(i, vector.get(i) - other.get(i));
        }
        return result;
    }

    template<Number R, Number T, size_t N>
    Vector<R, N> operator*(const Vector<R, N> &vector, const T& scalar) {
        return scalar * vector;
    }

    template<Number R, Number T, size_t N>
    Vector<R, N> operator*(const T& scalar, const Vector<R, N> &vector) {
        Vector<R, N> result;
        for (size_t i = 0; i < N; ++i) {
            result.set(i, vector.get(i) * scalar);
        }
        return result;
    }

    // Vector2

    template<Number T>
    Vector2<T> operator-(const Vector2<T> &vector) {
        Vector2<T> result(-vector.x(), -vector.y());
        return result;
    }

    template<Number T, Number T2>
    Vector2<T> operator+(const Vector2<T> &vector, const Vector2<T2> &other) {
        Vector2<T> result;
        for (size_t i = 0; i < 2; ++i) {
            result.set(i, vector.get(i) + other.get(i));
        }
        return result;
    }

    template<Number T, Number R>
    Vector2<R> operator*(const Vector2<R> &vector, const T& scalar) {
        return scalar * vector;
    }

    template<Number T, Number R>
    Vector2<R> operator*(const T& scalar, const Vector2<R> &vector) {
        Vector2<R> result;
        for (size_t i = 0; i < 2; ++i) {
            result.set(i, vector.get(i) * scalar);
        }
        return result;
    }

    // Vector3

    template<Number T, Number T2>
    Vector3<T> operator+(const Vector3<T> &vector, const Vector3<T2> &other) {
        Vector3<T> result;
        for (size_t i = 0; i < 3; ++i) {
            result.set(i, vector.get(i) + other.get(i));
        }
        return result;
    }

    template<Number T, Number R>
    Vector3<R> operator*(const Vector3<R> &vector, const T& scalar) {
        return scalar * vector;
    }

    template<Number R, Number T>
    Vector3<R> operator*(const T& scalar, const Vector3<R> &vector) {
        Vector3<R> result;
        for (size_t i = 0; i < 3; ++i) {
            result.set(i, vector.get(i) * scalar);
        }
        return result;
    }

    template <Number T, bool _includeMin = true, bool _includeMax = true>
    constexpr bool isInRange(const T& value, const T& min, const T& max) {
        if constexpr (_includeMin && _includeMax)
            return value >= min && value <= max;
        else if constexpr (_includeMin && !_includeMax)
            return value >= min && value < max;
        else if constexpr (!_includeMin && _includeMax)
            return value > min && value <= max;
        else
            return value > min && value < max;
    }
}
