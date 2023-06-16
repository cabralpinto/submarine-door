#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#define _USE_MATH_DEFINES
#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <vector>

namespace Ease {
    GLfloat linear(GLfloat t);
    GLfloat quadraticIn(GLfloat t);
    GLfloat quadraticOut(GLfloat t);
    GLfloat quadraticInOut(GLfloat t);
    GLfloat cubicIn(GLfloat t);
    GLfloat cubicOut(GLfloat t);
    GLfloat cubicInOut(GLfloat t);
    GLfloat quarticIn(GLfloat t);
    GLfloat quarticOut(GLfloat t);
    GLfloat quarticInOut(GLfloat t);
    GLfloat quinticIn(GLfloat t);
    GLfloat quinticOut(GLfloat t);
    GLfloat quinticInOut(GLfloat t);
    GLfloat sinusoidalIn(GLfloat t);
    GLfloat sinusoidalOut(GLfloat t);
    GLfloat sinusoidalInOut(GLfloat t);
    GLfloat exponentialIn(GLfloat t);
    GLfloat exponentialOut(GLfloat t);
    GLfloat exponentialInOut(GLfloat t);
    GLfloat circularIn(GLfloat t);
    GLfloat circularOut(GLfloat t);
    GLfloat circularInOut(GLfloat t);
}

class Animation {
    private:
    unsigned long start, duration, time;
    GLfloat first, last;
    bool started, done;
    GLfloat (*ease)(GLfloat);
    std::function<void(GLfloat)> callback;

    public:
    Animation(unsigned long start, unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), std::function<void(GLfloat)> callback);
    Animation(unsigned long start, unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), GLfloat *value);
    Animation(unsigned long start, unsigned long duration, GLfloat fixed, std::function<void(GLfloat)> callback);
    Animation(unsigned long start, unsigned long duration, GLfloat fixed, GLfloat *value);
    Animation(unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), std::function<void(GLfloat)> callback);
    Animation(unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), GLfloat *value);
    Animation(unsigned long duration, GLfloat fixed, std::function<void(GLfloat)> callback);
    Animation(unsigned long duration, GLfloat fixed, GLfloat *value);
    bool isDone();
    void reset();
    void tick(unsigned long delta);
};

class AnimationGroup {
    private:
    bool done;
    std::vector<Animation> animations;

    public:
    AnimationGroup(std::initializer_list<Animation> animations);
    bool isDone();
    void reset();
    void tick(unsigned long delta);
};

#endif