#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <GL/freeglut.h>

#include <functional>
#include <iostream>
#include <type_traits>
#include <variant>

template <typename T>
class DynamicValue {
    private:
    std::variant<T, std::function<T()>> getter;

    public:
    DynamicValue(const T& constant) : getter(constant){};
    template <typename F, typename = std::enable_if_t<std::is_invocable_v<F>>>
    DynamicValue(F&& function) : getter(function) {}
    DynamicValue(const T* pointer) : DynamicValue([pointer]() { return *pointer; }) {}
    DynamicValue(const DynamicValue& value) : getter(value.getter) {}
    DynamicValue(DynamicValue& value) : DynamicValue((const DynamicValue&) value) {}
    DynamicValue(DynamicValue&& value) : getter(std::move(value.getter)) {}
    ~DynamicValue() {}
    DynamicValue& operator=(const DynamicValue& value) {
        getter = value.getter;
        return *this;
    }
    T operator()() const { return getter.index() == 0 ? std::get<T>(getter) : std::get<std::function<T()>>(getter)(); }
};

struct Coordinates4D {
    union {
        struct {
            GLfloat x, y, z, w;
        };
        struct {
            GLfloat array[4];
        };
    };
    Coordinates4D() : Coordinates4D(0, 0, 0, 0) {}
    Coordinates4D(GLfloat x, GLfloat y, GLfloat z, GLfloat w) : x(x), y(y), z(z), w(w) {}
};

struct Coordinates3D {
    union {
        struct {
            GLfloat x, y, z;
        };
        struct {
            GLfloat array[3];
        };
    };

    Coordinates3D() : Coordinates3D(0, 0, 0) {}
    Coordinates3D(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}

    Coordinates3D operator+(Coordinates3D addend) { return {x + addend.x, y + addend.y, z + addend.z}; }
    Coordinates3D operator-(Coordinates3D addend) { return {x - addend.x, y - addend.y, z - addend.z}; }

    Coordinates4D toPoint() { return {x, y, z, 1}; }
    Coordinates4D toVector() { return {x, y, z, 0}; }
};

struct Coordinates2D {
    union {
        struct {
            GLfloat x, y;
        };
        struct {
            GLfloat array[2];
        };
    };
    Coordinates2D() : Coordinates2D(0, 0) {}
    Coordinates2D(GLfloat x, GLfloat y) : x(x), y(y) {}
};

struct Angle3D {
    union {
        struct {
            GLfloat theta, phi;
        };
        struct {
            GLfloat array[2];
        };
    };
    Angle3D() : Angle3D(0, 0) {}
    Angle3D(GLfloat theta, GLfloat phi) : theta(std::fmod(theta, 2 * M_PI)), phi(std::fmod(phi, M_PI_2)) {}
};

struct ColorRGB {
    union {
        struct {
            GLfloat r, g, b;
        };
        struct {
            GLfloat array[3];
        };
    };
    ColorRGB() : ColorRGB(0, 0, 0) {}
    ColorRGB(GLfloat r, GLfloat g, GLfloat b) : r(r), g(g), b(b) {}
};

struct ColorRGBA {
    union {
        struct {
            GLfloat r, g, b, a;
        };
        struct {
            GLfloat array[4];
        };
    };
    ColorRGBA() : ColorRGBA(0, 0, 0, 0) {}
    ColorRGBA(GLfloat r, GLfloat g, GLfloat b, GLfloat a) : r(r), g(g), b(b), a(a) {}
};

struct QuadraticAttenuation {
    union {
        struct {
            GLfloat kc, kl, kq;
        };
        struct {
            GLfloat array[3];
        };
    };
    QuadraticAttenuation() : QuadraticAttenuation(0, 0, 0) {}
    QuadraticAttenuation(GLfloat kc, GLfloat kl, GLfloat kq) : kc(kc), kl(kl), kq(kq) {}
};

#endif