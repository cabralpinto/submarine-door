#ifndef SHAPES_HPP
#define SHAPES_HPP

#define _USE_MATH_DEFINES

#include <GL/freeglut.h>
#include <assert.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

#include "structures.hpp"

struct Transformation {
    Coordinates3D parameters;
    std::function<void(Coordinates3D &)> getParameters;
    Transformation(Coordinates3D parameters) : parameters(parameters), getParameters(NULL) {}
    Transformation(std::function<void(Coordinates3D &)> getParameters) : parameters{0, 0, 0}, getParameters(getParameters) {}
    
    virtual void execute() {
        if (getParameters) getParameters(parameters);
    }
};

struct Translation : Transformation {
    Translation(Coordinates3D parameters) : Transformation(parameters) {}
    Translation(std::function<void(Coordinates3D &)> getParameters) : Transformation(getParameters){};
    
    void execute() {
        Transformation::execute();
        glTranslatef(parameters.x, parameters.y, parameters.z);
    }
};

struct Rotation : Transformation {
    Rotation(Coordinates3D parameters) : Transformation(parameters) {}
    Rotation(std::function<void(Coordinates3D &)> getParameters) : Transformation(getParameters){};
    
    void execute() {
        Transformation::execute();
        glRotatef(parameters.x, 1, 0, 0);
        glRotatef(parameters.y, 0, 1, 0);
        glRotatef(parameters.z, 0, 0, 1);
    }
};

struct Scale : Transformation {
    Scale(Coordinates3D parameters) : Transformation(parameters) {}
    Scale(std::function<void(Coordinates3D &)> getParameters) : Transformation(getParameters){};
    
    void execute() {
        Transformation::execute();
        glScalef(parameters.x, parameters.y, parameters.z);
    }
};

class CompoundShape;

class Shape {
    private:
    DynamicValue<ColorRGBA> color, ambient, diffuse, specular;
    DynamicValue<GLfloat> shininess;
    std::vector<std::shared_ptr<Transformation>> transformations;
    virtual void renderRaw() = 0;

    public:
    Shape();
    virtual Shape *clone() const = 0;
    virtual CompoundShape *clone(int times, std::function<Shape *(int, Shape *)> transform);
    Shape *setColor(ColorRGBA color);
    Shape *setMaterial(DynamicValue<ColorRGBA> ambient, DynamicValue<ColorRGBA> diffuse, DynamicValue<ColorRGBA> specular, DynamicValue<GLfloat> shininess);
    Shape *translate(Coordinates3D parameters);
    Shape *translate(std::function<void(Coordinates3D &)> getParameters);
    Shape *rotate(Coordinates3D parameters);
    Shape *rotate(std::function<void(Coordinates3D &)> getParameters);
    Shape *scale(Coordinates3D parameters);
    Shape *scale(std::function<void(Coordinates3D &)> getParameters);
    virtual void render();
};

class SimpleShape : public Shape {
    private:
    GLuint texture;
    int meshLevel;
    DynamicValue<bool> meshEnabled;
    virtual void generate() = 0;
    virtual int getQuadCount() const = 0;
    void createMesh(int level);

    protected:
    std::vector<GLfloat> vertices, normals, textureVertices, meshVertices, meshNormals, meshTextureVertices;
    virtual void renderRaw();

    public:
    SimpleShape();
    SimpleShape *setTexture(GLuint texture);
    SimpleShape *setMeshLevel(int meshLevel);
    SimpleShape *setMeshEnabled(DynamicValue<bool> meshEnabled);
};

class Cuboid : public SimpleShape {
    private:
    GLfloat width, height, length;

    int getQuadCount() const { return 6; }
    void generate();

    public:
    Cuboid(GLfloat width, GLfloat height, GLfloat length);
    Shape *clone() const;
};

class PrismWall : public SimpleShape {
    private:
    GLfloat radius, height;
    int sides, span;
    float offset;

    int getQuadCount() const { return span; }
    void generate();

    public:
    PrismWall(GLfloat radius, GLfloat height, int sides);
    PrismWall(GLfloat radius, GLfloat height, int sides, float offset, int span);
    Shape *clone() const;
};

class Sphere : public SimpleShape {
    private:
    GLfloat radius;
    int detail, spanX, spanY;
    float offsetX, offsetY;

    int getQuadCount() const { return spanY * spanX; }
    void generate();

    public:
    Sphere(GLfloat radius, int detail);
    Sphere(GLfloat radius, int detail, float offsetX, int spanX, float offsetY, int spanY);

    Shape *clone() const;
};

class Donut : public SimpleShape {
    private:
    GLfloat middleRadius, ringRadius;
    int detailXY, spanXY, detailZ, spanZ;
    float offsetXY, offsetZ;

    int getQuadCount() const { return spanXY * spanZ; }
    void generate();

    public:
    Donut(GLfloat innerRadius, GLfloat outterRadius, int detailXY, int detailZ);
    Donut(GLfloat innerRadius, GLfloat outterRadius, int detailXY, float offsetXY, int spanXY, int detailZ, float offsetZ, int spanZ);
    Shape *clone() const;
};

class Ring : public SimpleShape {
    private:
    GLfloat innerRadius, outterRadius, height;
    int detail, span;
    float offset;

    int getQuadCount() const { return 4 * span; }
    void generate();

    public:
    Ring(GLfloat innerRadius, GLfloat outterRadius, GLfloat height, int detail);
    Ring(GLfloat innerRadius, GLfloat outterRadius, GLfloat height, int detail, float offset, int span);
    Shape *clone() const;
};

class CompoundShape : public Shape {
    private:
    std::vector<std::shared_ptr<Shape>> shapes;
    void renderRaw();

    public:
    CompoundShape(std::initializer_list<Shape *> shapes);
    CompoundShape(std::vector<Shape *> shapes);
    Shape *clone() const;
    CompoundShape *clone(int times, Shape *(*transform)(int, Shape *) );
};

#endif