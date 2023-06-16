#include "observer.hpp"

// class Observer

Observer::Observer(GLfloat x, GLfloat y, GLfloat z, GLfloat theta, GLfloat phi, GLfloat sensitivity, GLfloat mass, GLfloat forceCoefficient, GLfloat dragCoefficient)
    : position{x, y, z}, angle{theta, phi}, sensitivity(sensitivity), mass(mass), forceCoefficient(forceCoefficient), dragCoefficient(dragCoefficient) {}

Coordinates3D Observer::getPosition() { return position; }

Coordinates3D Observer::getVelocity() { return velocity; }

Coordinates3D Observer::getFrontVector() { return frontVector; }

Coordinates3D Observer::getFocusPoint() { return {position.x + frontVector.x, position.y + frontVector.y, position.z + frontVector.z}; }

Angle3D Observer::getAngle() { return angle; }

void Observer::setX(GLfloat x) { position.x = x; }

void Observer::setY(GLfloat y) { position.y = y; }

void Observer::setZ(GLfloat z) { position.z = z; }

void Observer::setTheta(GLfloat theta) { angle.theta = theta; }

void Observer::setPhi(GLfloat phi) { angle.phi = phi; }

void Observer::setVelocity(GLfloat x, GLfloat y, GLfloat z) { velocity = {x, y, z}; }

void Observer::moveCamera(GLfloat x, GLfloat y) {
    angle.theta = std::fmod(angle.theta + x * sensitivity, 2 * M_PI);
    angle.phi -= y * sensitivity;
    if (angle.phi > M_PI_2 - 0.001) {
        angle.phi = M_PI_2 - 0.001;
    } else if (angle.phi < -M_PI_2 + 0.001) {
        angle.phi = -M_PI_2 + 0.001;
    }
}

void Observer::applyForce(GLfloat front, GLfloat right) {
    force = {
        (frontVector.x * front + rightVector.x * right) * forceCoefficient,
        (frontVector.y * front) * forceCoefficient,
        (frontVector.z * front + rightVector.y * right) * forceCoefficient,
    };
}

void Observer::updateVectors() {
    GLfloat xz = cos(angle.phi);
    frontVector.x = xz * cos(angle.theta);
    frontVector.y = sin(angle.phi);
    frontVector.z = xz * sin(angle.theta);
    rightVector.x = cos(angle.theta + M_PI_2);
    rightVector.y = sin(angle.theta + M_PI_2);
}

void Observer::updatePosition(unsigned long delta) {
    position = {
        position.x + (force.x / dragCoefficient) * delta + (force.x / dragCoefficient - velocity.x) * (mass / dragCoefficient) * (GLfloat) exp(-(dragCoefficient / mass) * delta) - (force.x / dragCoefficient - velocity.x) * (mass / dragCoefficient),
        position.y + (force.y / dragCoefficient) * delta + (force.y / dragCoefficient - velocity.y) * (mass / dragCoefficient) * (GLfloat) exp(-(dragCoefficient / mass) * delta) - (force.y / dragCoefficient - velocity.y) * (mass / dragCoefficient),
        position.z + (force.z / dragCoefficient) * delta + (force.z / dragCoefficient - velocity.z) * (mass / dragCoefficient) * (GLfloat) exp(-(dragCoefficient / mass) * delta) - (force.z / dragCoefficient - velocity.z) * (mass / dragCoefficient),
    };
    velocity = {
        (force.x / dragCoefficient) - (GLfloat) exp(-(dragCoefficient / mass) * delta) * (force.x / dragCoefficient - velocity.x),
        (force.y / dragCoefficient) - (GLfloat) exp(-(dragCoefficient / mass) * delta) * (force.y / dragCoefficient - velocity.y),
        (force.z / dragCoefficient) - (GLfloat) exp(-(dragCoefficient / mass) * delta) * (force.z / dragCoefficient - velocity.z),
    };
    force.x = force.y = force.z = 0;
}

void Observer::tick(unsigned long delta) {
    updateVectors();
    updatePosition(delta);
}