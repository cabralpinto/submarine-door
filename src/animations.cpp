#include "animations.hpp"

// namespace Ease

GLfloat Ease::linear(GLfloat t) {
    return t;
}

GLfloat Ease::quadraticIn(GLfloat t) {
    return t * t;
}

GLfloat Ease::quadraticOut(GLfloat t) {
    return -t * (t - 2);
}

GLfloat Ease::quadraticInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return t * t / 2;
    t--;
    return -(t * (t - 2) - 1) / 2;
}

GLfloat Ease::cubicIn(GLfloat t) {
    return t * t * t;
}

GLfloat Ease::cubicOut(GLfloat t) {
    t--;
    return t * t * t + 1;
}

GLfloat Ease::cubicInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return t * t * t / 2;
    t -= 2;
    return (t * t * t + 2) / 2;
}

GLfloat Ease::quarticIn(GLfloat t) {
    return t * t * t * t;
}

GLfloat Ease::quarticOut(GLfloat t) {
    t--;
    return -t * t * t * t + 1;
}

GLfloat Ease::quarticInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return t * t * t * t / 2;
    t -= 2;
    return -(t * t * t * t - 2) / 2;
}

GLfloat Ease::quinticIn(GLfloat t) {
    return t * t * t * t * t;
}

GLfloat Ease::quinticOut(GLfloat t) {
    t--;
    return t * t * t * t * t + 1;
}

GLfloat Ease::quinticInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return t * t * t * t * t / 2;
    t -= 2;
    return (t * t * t * t * t + 2) / 2;
}

GLfloat Ease::sinusoidalIn(GLfloat t) {
    return -cos(t * M_PI_2) + 1;
}

GLfloat Ease::sinusoidalOut(GLfloat t) {
    return sin(t * M_PI_2);
}

GLfloat Ease::sinusoidalInOut(GLfloat t) {
    return -(cos(M_PI * t) - 1) / 2;
}

GLfloat Ease::exponentialIn(GLfloat t) {
    return pow(2, 10 * (t - 1));
}

GLfloat Ease::exponentialOut(GLfloat t) {
    return -pow(2, -10 * t) + 1;
}

GLfloat Ease::exponentialInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return pow(2, 10 * (t - 1)) / 2;
    t--;
    return (-pow(2, -10 * t) + 2) / 2;
}

GLfloat Ease::circularIn(GLfloat t) {
    return -(sqrt(1 - t * t) - 1);
}

GLfloat Ease::circularOut(GLfloat t) {
    t--;
    return sqrt(1 - t * t);
}

GLfloat Ease::circularInOut(GLfloat t) {
    t *= 2;
    if (t < 1) return -(sqrt(1 - t * t) - 1) / 2;
    t -= 2;
    return (sqrt(1 - t * t) + 1) / 2;
}

// class Animation

Animation::Animation(unsigned long start, unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), std::function<void(GLfloat)> callback)
    : start(start), duration(duration), time(0), first(first), last(last), started(false), done(false), ease(ease), callback(callback) {}

Animation::Animation(unsigned long start, unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), GLfloat *value)
    : Animation(start, duration, first, last, ease, [value](GLfloat newValue) { *value = newValue; }) {}

Animation::Animation(unsigned long start, unsigned long duration, GLfloat fixed, std::function<void(GLfloat)> callback)
    : Animation(
          start, duration, fixed, fixed, [](GLfloat) { return (GLfloat) 0; }, callback) {}

Animation::Animation(unsigned long start, unsigned long duration, GLfloat fixed, GLfloat *value)
    : Animation(
          start, duration, fixed, fixed, [](GLfloat) { return (GLfloat) 0; }, value) {}

Animation::Animation(unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), std::function<void(GLfloat)> callback)
    : Animation(0, duration, first, last, ease, callback) {}

Animation::Animation(unsigned long duration, GLfloat first, GLfloat last, GLfloat (*ease)(GLfloat), GLfloat *value)
    : Animation(0, duration, first, last, ease, value) {}

Animation::Animation(unsigned long duration, GLfloat fixed, std::function<void(GLfloat)> callback)
    : Animation(0, duration, fixed, callback) {}

Animation::Animation(unsigned long duration, GLfloat fixed, GLfloat *value)
    : Animation(0, duration, fixed, value) {}

bool Animation::isDone() { return done; }

void Animation::reset() {
    time = 0;
    started = done = false;
}

void Animation::tick(unsigned long delta) {
    if (!started) {
        started = true;
        delta = 0;
    }
    time += delta;
    if (time <= start + duration) {
        if (time >= start) {
            callback(first + (ease((time - start + 0.) / duration) * (last - first)));
        }
    } else if (!done) {
        callback(last);
        done = true;
    }
}

// class AnimationGroup

AnimationGroup::AnimationGroup(std::initializer_list<Animation> animations)
    : done(false), animations(animations) {}

bool AnimationGroup::isDone() { return done; }

void AnimationGroup::reset() {
    for_each(animations.begin(), animations.end(), [](Animation &animation) { animation.reset(); });
    done = false;
}

void AnimationGroup::tick(unsigned long delta) {
    done = accumulate(animations.begin(), animations.end(), true, [delta](bool done, Animation &animation) {
        animation.tick(delta);
        return done && animation.isDone();
    });
}
