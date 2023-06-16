#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#define _USE_MATH_DEFINES

#include <cmath>

#include "structures.hpp"

class Observer {
    private:
    Coordinates3D position, velocity, force, drag, frontVector;
    Angle3D angle;
    Coordinates2D rightVector;
    GLfloat sensitivity, mass, forceCoefficient, dragCoefficient;

    public:
    Observer(GLfloat x, GLfloat y, GLfloat z, GLfloat theta, GLfloat phi, GLfloat sensitivity, GLfloat mass, GLfloat forceCoefficient, GLfloat dragCoefficient);
    Coordinates3D getPosition();
    Coordinates3D getVelocity();
    Coordinates3D getFrontVector();
    Coordinates3D getFocusPoint();
    Angle3D getAngle();
    void setX(GLfloat x);
    void setY(GLfloat y);
    void setZ(GLfloat z);
    void setTheta(GLfloat theta);
    void setPhi(GLfloat phi);
    void setVelocity(GLfloat x, GLfloat y, GLfloat z);
    void moveCamera(GLfloat x, GLfloat y);
    void applyForce(GLfloat front, GLfloat right);
    void updateVectors();
    void updatePosition(unsigned long delta);
    void tick(unsigned long delta);
};

#endif