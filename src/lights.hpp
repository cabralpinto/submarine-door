#include <GL/freeglut.h>
#include <assert.h>

#include <iostream>

#include "structures.hpp"

class Light {
    private:
    static int count;
    DynamicValue<ColorRGBA> ambient, diffuse, specular;
    DynamicValue<Coordinates4D> position;
    DynamicValue<bool> on;

    protected:
    int id;
    Light(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates4D> position, DynamicValue<bool> on = true)
        : Light(ColorRGBA{0, 0, 0, 1}, diffuse, specular, position, on) {}
    Light(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates4D> position, DynamicValue<bool> on = true)
        : id(GL_LIGHT0 + count), ambient(ambient), diffuse(diffuse), specular(specular), position(position), on(on) {
        assert(++count <= GL_MAX_LIGHTS);
    }

    public:
    ~Light() { --count; }

    bool isOn() const { return on(); }

    virtual void render() {
        if (on()) {
            glEnable(id);
            glLightfv(id, GL_AMBIENT, ambient().array);
            glLightfv(id, GL_DIFFUSE, diffuse().array);
            glLightfv(id, GL_SPECULAR, specular().array);
            glLightfv(id, GL_POSITION, position().array);
        } else {
            glDisable(id);
        }
    }
};

int Light::count = 0;

class PointLight : public Light {
    private:
    QuadraticAttenuation attenuation;

    public:
    PointLight(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<bool> on = true)
        : PointLight(ColorRGBA{0, 0, 0, 1}, diffuse, specular, position, on) {}
    PointLight(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<bool> on = true)
        : PointLight(ambient, diffuse, specular, position, {1, 0, 0}, on) {}
    PointLight(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, QuadraticAttenuation attenuation, DynamicValue<bool> on = true)
        : PointLight(ColorRGBA{0, 0, 0, 1}, diffuse, specular, position, attenuation, on) {}
    PointLight(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, QuadraticAttenuation attenuation, DynamicValue<bool> on = true)
        : Light(ambient, diffuse, specular, [position] { return position().toPoint(); }, on), attenuation(attenuation) {}

    virtual void render() {
        Light::render();
        glLightf(id, GL_CONSTANT_ATTENUATION, attenuation.kc);
        glLightf(id, GL_LINEAR_ATTENUATION, attenuation.kl);
        glLightf(id, GL_QUADRATIC_ATTENUATION, attenuation.kq);
    }
};

class SpotLight : public PointLight {
    private:
    DynamicValue<Coordinates3D> direction;
    GLfloat cutoff, exponent;

    public:
    SpotLight(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<Coordinates3D> direction, GLfloat cutoff, GLfloat exponent, DynamicValue<bool> on = true)
        : SpotLight(ColorRGBA{0, 0, 0, 1}, diffuse, specular, position, direction, cutoff, exponent, on) {}
    SpotLight(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<Coordinates3D> direction, GLfloat cutoff, GLfloat exponent, DynamicValue<bool> on = true)
        : SpotLight(ambient, diffuse, specular, position, direction, cutoff, exponent, {1, 0, 0}, on) {}
    SpotLight(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<Coordinates3D> direction, GLfloat cutoff, GLfloat exponent, QuadraticAttenuation attenuation, DynamicValue<bool> on = true)
        : SpotLight(ColorRGBA{0, 0, 0, 1}, diffuse, specular, position, direction, cutoff, exponent, attenuation, on) {}
    SpotLight(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> position, DynamicValue<Coordinates3D> direction, GLfloat cutoff, GLfloat exponent, QuadraticAttenuation attenuation, DynamicValue<bool> on = true)
        : PointLight(ambient, diffuse, specular, position, attenuation, on), direction(direction), cutoff(cutoff), exponent(exponent) {}

    void setDirection(Coordinates3D direction) { this->direction = direction; }

    void init() {
        PointLight::render();
        glLightf(id, GL_SPOT_CUTOFF, cutoff);
        glLightf(id, GL_SPOT_EXPONENT, exponent);
    }

    void render() {
        PointLight::render();
        glLightf(id, GL_SPOT_CUTOFF, cutoff);
        glLightf(id, GL_SPOT_EXPONENT, exponent);
        glLightfv(id, GL_SPOT_DIRECTION, direction().array);
    }
};

class DirectionalLight : public Light {
    public:
    DirectionalLight(DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> direction, DynamicValue<bool> on = true)
        : DirectionalLight(ColorRGBA{0, 0, 0, 1}, diffuse, specular, direction, on) {}
    DirectionalLight(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<Coordinates3D> direction, DynamicValue<bool> on = true)
        : Light(ambient, diffuse, specular, [direction] { return direction().toVector(); }, on) {}
};
