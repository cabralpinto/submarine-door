/* INCLUDE/DEFINE */

#define _USE_MATH_DEFINES

#include <GL/glew.h>
// glew must be included first
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

#include "RgbImage.h"
#include "animations.hpp"
#include "keys.hpp"
#include "lights.hpp"
#include "observer.hpp"
#include "shaders.hpp"
#include "shapes.hpp"
#include "structures.hpp"

#define BLUE 0.0, 0.0, 1.0, 1.0
#define TRANSPARENT_BLUE 0.0, 0.0, 1.0, 0.5
#define RED 1.0, 0.0, 0.0, 1.0
#define YELLOW 1.0, 1.0, 0.0, 1.0
#define GREEN 0.0, 1.0, 0.0, 1.0
#define WHITE 1.0, 1.0, 1.0, 1.0
#define BLACK 0.0, 0.0, 0.0, 1.0
#define GRAY 0.5, 0.5, 0.5, 1.0
#define WHITE_METAL \
    ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f}, ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f}, ColorRGBA{1.0f, 1.0f, 1.0f, 1.0f}, 76.8f
#define GRAY_METAL \
    ColorRGBA{0.7f, 0.7f, 0.7f, 1.0f}, ColorRGBA{0.7f, 0.7f, 0.7f, 1.0f}, ColorRGBA{0.9f, 0.9f, 0.9f, 1.0f}, 76.8f
#define DARK_GRAY_METAL \
    ColorRGBA{0.25f, 0.25f, 0.25f, 1.0f}, ColorRGBA{0.4f, 0.4f, 0.4f, 1.0f}, ColorRGBA{0.774597f, 0.774597f, 0.774597f, 1.0f}, 76.8f
#define RED_METAL \
    ColorRGBA{1.f, 0.20725f, 0.20725f, 1}, ColorRGBA{1.0f, 0.5f, 0.5f, 1}, ColorRGBA{1.0f, 0.5f, 0.5f, 1}, 76.8f
#define BLACK_RUBBER \
    ColorRGBA{0.02f, 0.02f, 0.02f, 1.0f}, ColorRGBA{0.01f, 0.01f, 0.01f, 1.0f}, ColorRGBA{0.4f, 0.4f, 0.4f, 1.0f}, 10.0f
#define YELLOW_METAL \
    ColorRGBA{0.5f, 0.5f, 0.0f, 1.0f}, ColorRGBA{0.5f, 0.5f, 0.0f, 1.0f}, ColorRGBA{0.60f, 0.60f, 0.50f, 1.0f}, 32.0f
#define GLASS \
    ColorRGBA{0.1f, 0.18725f, 0.1745f, 0.4f}, ColorRGBA{0.396f, 0.74151f, 0.69102f, 0.4f}, ColorRGBA{0.297254f, 0.30829f, 0.306678f, 0.8f}, 12.8f
#define SILVER \
    ColorRGBA{0.773911f, 0.773911f, 0.773911f, 1.0f}, ColorRGBA{0.773911f, 0.773911f, 0.773911f, 1.0f}, ColorRGBA{0.773911f, 0.773911f, 0.773911f, 1.0f}, 100.f

/* CLASSES */

/* GLOBALS */

// assets
// SpotLight flashlight;
std::map<std::string, GLuint> textures;
std::map<std::string, Shader> shaders;
std::vector<std::unique_ptr<Light>> lights;
std::unique_ptr<Shape> skybox, scene;
std::vector<AnimationGroup> animations;

// screen
GLint screenWidth = 1280, screenHeight = 720, screenCenterX = screenWidth / 2, screenCenterY = screenHeight / 2;

// observer
Observer observer = Observer(0, 0, 14, -M_PI_2, 0, 0.0003, 1000, 0.35, 5);
GLfloat fov = 75, renderDistance = 100;

// scene
GLfloat doorAngle = 0, valveAngle = 0, lockProgress = 1, solidness = 1, skyboxAngle = 0;

// time
std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now(), currentFrameTime;
double fps;

// control
bool
    forwardKeyPressed = false,
    leftwardKeyPressed = false,
    backwardKeyPressed = false,
    rightwardKeyPressed = false,
    wireframeOn = false,
    cullingOn = true,
    lightingOn = true,
    axesOn = false,
    flashlightOn = true,
    skyboxOn = true,
    meshOn = false,
    debugInfoOn = true,
    instructionsOn = true,
    animationPlaying = false;
int currentAnimation = 0;
std::string currentShader = "phong";

// keys
std::vector<Key> keys = {
    Key(
        'W', "Move forward", [] { forwardKeyPressed = true; }, [] { forwardKeyPressed = false; }),
    Key(
        'A', "Move left", [] { leftwardKeyPressed = true; }, [] { leftwardKeyPressed = false; }),
    Key(
        'S', "Move back", [] { backwardKeyPressed = true; }, [] { backwardKeyPressed = false; }),
    Key(
        'D', "Move right", [] { rightwardKeyPressed = true; }, [] { rightwardKeyPressed = false; }),
    Key('F', "Toggle flashlight", flashlightOn),
    Key('B', "Toggle skybox", skyboxOn),
    Key('Z', "Toggle wireframe", wireframeOn),
    Key('X', "Toggle axes", axesOn),
    Key('L', "Toggle lighting", lightingOn),
    Key('C', "Toggle culling", cullingOn),
    Key('M', "Toggle mesh", meshOn),
    Key('P', "Turn on Phong shading", [] { currentShader = "phong"; }),
    Key('G', "Turn on Gouraud shading", [] { currentShader = "gouraud"; }),
    Key('H', "Turn off shaders", [] { currentShader = ""; }),
    Key(GLUT_KEY_F3, "F3", "Toggle debug informnation", debugInfoOn),
    Key(GLUT_KEY_F4, "F4", "Toggle instructions", instructionsOn),
    Key(GLUT_KEY_F11, "F11", "Toggle fullscreen", glutFullScreenToggle),
    Key(' ', "Spacebar", "Play/pause animation", [] {
        animationPlaying = !animationPlaying;
        if (animationPlaying) observer.setVelocity(0, 0, 0);
    }),
    Key(GLUT_KEY_LEFT, "Left Arrow", "Go to previous animation", [] {
        currentAnimation = ((currentAnimation - 1) % (int) animations.size() + animations.size()) % (int) animations.size();
        animations[currentAnimation].reset();
        animationPlaying = false;
    }),
    Key(GLUT_KEY_RIGHT, "Right Arrow", "Go to next animation", [] {
        currentAnimation = (currentAnimation + 1) % animations.size();
        animations[currentAnimation].reset();
        animationPlaying = false;
    }),
    Key((char) 27, "ESC", "Exit", [] { exit(0); }),
};

/* INITIALIZATION FUNCTIONS */

std::string readFile(std::string path) {
    std::ostringstream stream;
    stream << std::ifstream(path).rdbuf();
    return stream.str();
}

void loadTexture(GLuint *texture, std::string filename) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    RgbImage img;
    img.LoadBmpFile(filename.c_str());
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, img.GetNumCols(), img.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.ImageData());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void initializeTextures() {
    for (std::string name : {"skybox"}) {
        loadTexture(&textures[name], "res/textures/" + name + ".bmp");
    }
}

void initializeLights() {
    // ambient light
    const GLfloat ambient[] = {0.1, 0.1, 0.1, 1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    // point light
    lights.emplace_back(new DirectionalLight(ColorRGBA{0.4, 0.4, 0.4, 1}, ColorRGBA{0.2, 0.2, 0.2, 1}, [] { GLfloat plusOffset = (skyboxAngle + 22) * M_PI / 180; return Coordinates3D{(GLfloat) sin(plusOffset), 1, (GLfloat) cos(plusOffset)}; }));
    // flashlight
    lights.emplace_back(new SpotLight(
        ColorRGBA{1, 1, 1, 1}, ColorRGBA{1, 1, 1, 1}, [] { return observer.getPosition() - observer.getFrontVector(); }, [] { return observer.getFrontVector(); }, 20, 1, {1, 0.05, 0.025}, &flashlightOn));
}

void initializeShaders() {
    // enable compatibility mode
    GLEW_ARB_vertex_shader;
    GLEW_ARB_fragment_shader;
    // load phong shader
    auto lightsOn = DynamicValue<std::vector<int>>([] {
        std::vector<int> lightsOn;
        std::transform(lights.begin(), lights.end(), std::back_inserter(lightsOn), [](auto &light) { return light->isOn(); });
        return lightsOn;
    });
    shaders.emplace("phong", Shader(readFile("res/shaders/phong.vert"), readFile("res/shaders/phong.frag"), {{"maxLights", DynamicValue<int>(lights.size())}, {"lightsOn", lightsOn}, {"solidness", DynamicValue<float>(&solidness)}}));
    shaders.emplace("gouraud", Shader(readFile("res/shaders/gouraud.vert"), readFile("res/shaders/gouraud.frag"), {{"maxLights", DynamicValue<int>(lights.size())}, {"lightsOn", lightsOn}}));
}

void initializeShapes() {
    skybox = std::unique_ptr<Shape>((new Sphere(1, 20))->setTexture(textures["skybox"])->rotate([](Coordinates3D &parameters) { parameters = {0, skyboxAngle, 0}; }));

    // clang-format off
    auto valve = new CompoundShape{
        // stem
        (new CompoundShape{
            (new PrismWall(0.1, 1, 40))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(4)
                ->setMaterial(WHITE_METAL),
            (new Sphere(0.1, 20, 0, 10, 0, 20))
                ->setMaterial(WHITE_METAL)
                ->translate({0, 0, 0.5})
        }),
        // wheel
        (new CompoundShape{
            // ring
            (new Donut(0.6, 0.8, 41, 10))
                ->setMaterial(RED_METAL)
                ->setColor({RED}),
            // spokes
            (new PrismWall(0.05, 0.7, 15, 0, 15))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(4)
                ->setMaterial(YELLOW_METAL)
                ->setColor({YELLOW})
                ->clone(3, [](int index, Shape *spoke) {
                    return spoke
                        ->rotate({0, (GLfloat) index * 360 / 3, 0})
                        ->translate({0, 0, 0.35});
                })
                ->rotate({-90, 0, 0})
        })->translate({0, 0, 0.45})
    };
    
    auto window = new CompoundShape{
        // border
        (new Ring(0.8, 1, 0.1, 40))
            ->setMeshEnabled(&meshOn)
            ->setMeshLevel(3)
            ->setMaterial(DARK_GRAY_METAL)
            ->setColor({WHITE}),
        // bolts
        (new Sphere(0.05, 16, 0, 8, 0, 16))
            ->setMaterial(SILVER)
            ->setColor({YELLOW})
            ->clone(10, [](int index, Shape *bolt) {
                return bolt
                    ->rotate({0, (GLfloat) index * 360 / 10, 0})
                    ->translate({0, 0, 0.9})
                    ->rotate({90, 0, 0});
            })
            ->translate({0, 0, 0.05})
            ->rotate({-90, 0, 0}),
        // glass
        (new Sphere(0.8, 40, 0, 20, 0, 40))
            ->setMaterial(GLASS)
            ->setColor({TRANSPARENT_BLUE})
            ->scale({1, 1, 0.35})
    };

    auto hinge = new CompoundShape{
        // pin
        (new PrismWall(0.4, 1.2, 16, 0, 16))
            ->setMaterial(RED_METAL)
            ->setColor({RED}),
        // pin ends
        (new Donut(0.4, 1.2, 20, 5, 5, 16, 0, 16))
            ->setMaterial(RED_METAL)
            ->setColor({RED})
            ->translate({0, 0.8, 0})
            ->rotate({90, 90, 0})
            ->clone(2, [](int index, Shape *pinEnd) {
                return pinEnd
                    ->translate({0, (GLfloat) (0.6 - (1.2 * (index == 0))), 0})
                    ->rotate({(GLfloat) 180 * (index == 0), 0, 0});
            }),
        // kuckles
        (new CompoundShape{
            (new Ring(0.4, 0.5, 0.5, 16, 0, 16))
                ->setMaterial(YELLOW_METAL)
                ->setColor({YELLOW}),
            (new Donut(0.6, 1, 20, 15, 5, 16, 0, 16))
                ->setMaterial(YELLOW_METAL)
                ->setColor({YELLOW})
                ->translate({0.4, 0.8, 0})
        })->clone(2, [](int index, Shape *knuckle) {
            return knuckle
                ->rotate([](Coordinates3D &parameters) { parameters = {0, 0, -doorAngle}; })
                ->translate({0, 0, (GLfloat) (0.35 - (0.7 * (index == 0)))});
        })
    };

    auto lock = new CompoundShape{
        (new CompoundShape{
            (new PrismWall(0.25, 1, 20))->setMaterial(WHITE_METAL),
            (new Sphere(0.25, 10, 0, 5, 0, 10))
                ->setMaterial(WHITE_METAL)
                ->translate({0, 0, 0.5})
        })
            ->clone(5, [](int i, Shape *cylinder) {
                return cylinder
                    ->translate([i](Coordinates3D &parameters) {
                        parameters = {0, -0.7f * i, (lockProgress >= (i + 0.) / 5) * ((lockProgress >= (i + 1.f) / 5) ? 1 : (lockProgress * 5 - i))};
                    });
            })
            ->rotate({0, 90, 0})
            ->translate({0, 1.4, -0.5})
    };

    auto frame = new CompoundShape{
        // side bars
        (new CompoundShape{
            (new Cuboid(0.26, 1.6, 0.06))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(DARK_GRAY_METAL),
            (new Cuboid(0.10, 1.6, 0.02))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(DARK_GRAY_METAL)
                ->translate({-0.18, 0, -0.02})
        })->clone(2, [](int index, Shape *bar) {
            return bar
                ->translate({(0.8f - (1.6f * (index == 0))), 0, 0})
                ->rotate({0, 0, 180.f * (index == 0)});
        }),
        // side bolts
        (new Sphere(0.06, 20, 0, 10, 0, 20))
            ->setColor({YELLOW})
            ->setMaterial(SILVER)
            ->clone(2, [](int i, Shape *bolt) {
                return (Shape *) bolt
                    ->translate({(0.8f - (1.6f * (i == 0))), 0, 0})
                    ->clone(2 + 2 * i, [i](int j, Shape *bolt) {
                        return bolt->translate({0, 0.65f - 1.3f * (j + (i == 0)) / 3, 0});
                    });
            })
            ->translate({0, 0, 0.03}),
        // top/bottom bars & bolts
        (new CompoundShape{
            // bar
            (new Ring(0.67, 0.93, 0.06, 40, 0, 20))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(DARK_GRAY_METAL),
            (new Ring(0.57, 0.67, 0.02, 40, 0, 20))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(DARK_GRAY_METAL)
                ->translate({0, 0, -0.02}),
            // bolts
            (new Sphere(0.06, 20, 0, 10, 0, 20))
                ->setColor({YELLOW})
                ->setMaterial(SILVER)
                ->clone(4, [](int index, Shape *bolt) {
                    return bolt
                        ->rotate({0, (index + 1.f) * 180 / 5 - 90, 0})
                        ->translate({0, 0, 0.8})
                        ->rotate({90, 0, 0});
                })
                ->translate({0, 0, 0.03})
                ->rotate({-90, 0, 0})
        })->clone(2, [](int index, Shape *bar) {
            return bar
                ->translate({0, (0.8f - (1.6f * (index == 0))), 0})
                ->rotate({180.f * (index == 0), 180.f * (index == 0), 0});
        })
    };

    auto panel = new CompoundShape{
        // middle panel
        new CompoundShape{
            (new Cuboid(1.28, 1.6, 0.04))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(GRAY_METAL)
                ->setColor({GREEN})
                ->translate({0, 0, 0.04}),
            (new Cuboid(1.04, 1.6, 0.17))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(6)
                ->setMaterial(GRAY_METAL)
                ->setColor({BLUE})
                ->translate({0, 0, -0.065})
        },
        // top/bottom panels
        (new CompoundShape{
            (new Ring(0, 0.64, 0.04, 40, 0, 20))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(5)
                ->setMaterial(GRAY_METAL)
                ->setColor({GREEN})
                ->translate({0, 0, 0.04}),
            (new Ring(0, 0.52, 0.17, 40, 0, 20))
                ->setMeshEnabled(&meshOn)
                ->setMeshLevel(5)
                ->setMaterial(GRAY_METAL)
                ->setColor({BLUE})
                ->translate({0, 0, -0.065})
        })->clone(2, [](int i, Shape *panel) {
            return panel
                ->translate({0, (0.8f - (1.6f * (i == 0))), 0})
                ->rotate({0, 0, 180.f * (i == 0)})
                ;
        })
    };

    scene = std::unique_ptr<Shape>((new CompoundShape{
        frame->translate({0, 0, 0.03}),
        hinge
            ->translate({-0.75, 0, 0.06})
            ->scale({0.12, 0.12, 0.12})
            ->translate({0, 0, 0.8})
            ->rotate({-90, 0, 0})
            ->clone(2, [](int i, Shape *hinge) {
                return hinge ->translate({0, 0, 5.f - 10.f * (i == 0)});
            }),
        (new CompoundShape{
            panel,
            window
                ->translate({0, 0.8, 0.06})
                ->scale({0.35, 0.35, 0.35})
                ->translate({0, 0, 0.05}),
            valve
                ->translate({0.4, 0, 0.06})
                ->scale({0.3, 0.3, 0.3})
                ->translate({0, 0, 0.5})
                ->rotate([](Coordinates3D &parameters) { parameters = {0, 0, valveAngle}; }),
            lock
                ->translate({0.52, 0, -0.025})
                ->scale({0.1, 0.1, 0.1})
        })
            ->translate({-0.75, 0, 0.156})
            ->rotate([](Coordinates3D &parameters) { parameters = {0, -doorAngle, 0}; })
            ->translate({0.75, 0, -0.156})
    })->scale({4, 4, 4}));
    
    // clang-format on
}

void initializeAnimations() {
    auto setObserverX = [](double x) { observer.setX(x); };
    auto setObserverY = [](double y) { observer.setY(y); };
    auto setObserverZ = [](double z) { observer.setZ(z); };
    auto setObserverTheta = [](double theta) { observer.setTheta(theta); observer.updateVectors(); };
    auto setObserverPhi = [](double phi) { observer.setPhi(phi); observer.updateVectors(); };
    animations = {
        {
            // maintain valve unrotated
            Animation(1050, 0, &valveAngle),
            // maintain lock closed
            Animation(1050, 1, &lockProgress),
            // maintain door closed
            Animation(15050, 0, &doorAngle),
            // go to valve
            Animation(1050, 0, 3.7245, Ease::quinticInOut, setObserverX),
            Animation(1050, 0, 0.4326, Ease::quinticInOut, setObserverY),
            Animation(1050, 14, 5.2588, Ease::quinticInOut, setObserverZ),
            Animation(1050, 3 * M_PI_2, -2.0418 + 2 * M_PI, Ease::quinticInOut, setObserverTheta),
            Animation(1050, 0, -0.1119, Ease::quinticInOut, setObserverPhi),
            // maintain spectator position
            Animation(1050, 2450, 3.7245, setObserverX),
            Animation(1050, 2450, 0.4326, setObserverY),
            Animation(1050, 2450, 5.2588, setObserverZ),
            Animation(1050, 2450, -2.0418, setObserverTheta),
            Animation(1050, 2450, -0.1119, setObserverPhi),
            // spin valve
            Animation(1050, 6475, 0, 5400, Ease::cubicIn, &valveAngle),
            Animation(7525, 6475, 0, 360, Ease::cubicOut, &valveAngle),
            // maintain valve unrotated
            Animation(14000, 4900, 0, &valveAngle),
            // open lock
            Animation(1050, 12950, 1, 0, Ease::sinusoidalInOut, &lockProgress),
            // maintain lock open
            Animation(14000, 4900, 0, &lockProgress),
            // go to lock
            Animation(3500, 525, 3.7245, 5, Ease::quinticIn, setObserverX),
            Animation(4025, 525, 5, 3.3553, Ease::quinticOut, setObserverX),
            Animation(3500, 525, 0.4326, 0.1760, Ease::quinticIn, setObserverY),
            Animation(4025, 525, 0.1760, -0.0807, Ease::quinticOut, setObserverY),
            Animation(3500, 525, 5.2588, -0.1430, Ease::quinticIn, setObserverZ),
            Animation(4025, 525, -0.1430, -3.6262, Ease::quinticOut, setObserverZ),
            Animation(3500, 525, -2.0418, -4.3809, Ease::quinticIn, setObserverTheta),
            Animation(3500, 525, -0.1119, -0.0318, Ease::quinticIn, setObserverPhi),
            // maintain spectator position
            Animation(4550, 2450, 3.3553, setObserverX),
            Animation(4550, 2450, -0.0807, setObserverY),
            Animation(4550, 2450, -3.6262, setObserverZ),
            Animation(4550, 2450, -4.3809, setObserverTheta),
            Animation(4550, 2450, -0.0318, setObserverPhi),
            // go to top position
            Animation(7000, 525, 3.3553, 5, Ease::quinticIn, setObserverX),
            Animation(7525, 525, 5, 6.5451, Ease::quinticOut, setObserverX),
            Animation(7000, 525, -0.0807, 0.1760, Ease::quinticIn, setObserverY),
            Animation(7525, 525, 0.1760, 2.6621, Ease::quinticOut, setObserverY),
            Animation(7000, 525, -3.6262, -0.1430, Ease::quinticIn, setObserverZ),
            Animation(7525, 525, -0.1430, 8.3658, Ease::quinticOut, setObserverZ),
            Animation(7000, 1050, -4.3809, -2.1264, Ease::quinticInOut, setObserverTheta),
            Animation(7000, 1050, -0.0318, -0.1854, Ease::quinticInOut, setObserverPhi),
            // maintain spectator position
            Animation(8050, 5600, 6.5451, setObserverX),
            Animation(8050, 5600, 2.6621, setObserverY),
            Animation(8050, 5600, 8.3658, setObserverZ),
            Animation(8050, 5600, -2.1264, setObserverTheta),
            Animation(8050, 5600, -0.1854, setObserverPhi),
            // go to initial position
            Animation(13650, 1050, 6.5451, 0, Ease::quinticInOut, setObserverX),
            Animation(13650, 1050, 2.6621, 2.6621, Ease::quinticInOut, setObserverY),
            Animation(13650, 1050, 8.3658, 14, Ease::quinticInOut, setObserverZ),
            Animation(13650, 1050, -2.1264, 3 * M_PI_2 - 2 * M_PI, Ease::quinticInOut, setObserverTheta),
            Animation(13650, 1050, -0.1854, -0.1854, Ease::quinticInOut, setObserverPhi),
            // maintain spectator position
            Animation(14700, 4200, 0, setObserverX),
            Animation(14700, 4200, 2.6621, setObserverY),
            Animation(14700, 4200, 14, setObserverZ),
            Animation(14700, 4200, 3 * M_PI_2, setObserverTheta),
            Animation(14700, 4200, -0.1854, setObserverPhi),
            // open door
            Animation(15050, 1050, 0, 135, Ease::quinticInOut, &doorAngle),
            // maintain door open
            Animation(16100, 2800, 135, &doorAngle),
            // go through door
            Animation(16800, 2100, 0, 0, Ease::quinticInOut, setObserverX),
            Animation(16800, 2100, 2.6621, 0, Ease::quinticInOut, setObserverY),
            Animation(16800, 2100, 14, -14, Ease::quinticInOut, setObserverZ),
            Animation(16800, 2100, 3 * M_PI_2, M_PI_2, Ease::quinticInOut, setObserverTheta),
            Animation(16800, 2100, -0.1854, 0, Ease::quinticInOut, setObserverPhi),
        },
        {
            Animation(150, 0, 14, Ease::sinusoidalOut, setObserverX),
            Animation(150, 150, 14, 0, Ease::sinusoidalIn, setObserverX),
            Animation(300, 150, 0, -14, Ease::sinusoidalOut, setObserverX),
            Animation(450, 150, -14, 0, Ease::sinusoidalIn, setObserverX),
            Animation(600, 0, setObserverY),
            Animation(150, 14, 0, Ease::sinusoidalIn, setObserverZ),
            Animation(150, 150, 0, -14, Ease::sinusoidalOut, setObserverZ),
            Animation(300, 150, -14, 0, Ease::sinusoidalIn, setObserverZ),
            Animation(450, 150, 0, 14, Ease::sinusoidalOut, setObserverZ),
            Animation(600, 3 * M_PI_2, -M_PI_2, Ease::linear, setObserverTheta),
            Animation(600, 0, setObserverPhi),
        },
        {
            Animation(3100, 0, setObserverX),
            Animation(3100, 0, setObserverY),
            Animation(3100, 14, setObserverZ),
            Animation(3100, -M_PI_2, setObserverTheta),
            Animation(3100, 0, setObserverPhi),
            Animation(1050, 0, 135, Ease::quinticInOut, &doorAngle),
            Animation(1050, 1000, 135, &doorAngle),
            Animation(2050, 1050, 135, 0, Ease::quinticInOut, &doorAngle),
        },
        {
            Animation(1050, 1, 0, Ease::quinticInOut, &solidness),
            Animation(1050, 1000, 0, &solidness),
            Animation(2050, 1050, 0, 1, Ease::quinticInOut, &solidness),
        },
        {
            Animation(5000, 0, 360, Ease::quinticInOut, &skyboxAngle),
        }};
}

/* KEYBOARD/MOUSE EVENT FUNCTIONS */

template <class T>
void onKeyDown(T value, int x, int y) {
    for (const auto &key : keys) {
        if (key == value) {
            key.down();
            break;
        }
    }
}

template <class T>
void onKeyUp(T value, int x, int y) {
    for (const auto &key : keys) {
        if (key == value) {
            key.up();
            break;
        }
    }
}

void onMouseMove(int x, int y) {
    glutWarpPointer(screenCenterX, screenCenterY);
    if (!animationPlaying) {
        observer.moveCamera(x - screenCenterX, y - screenCenterY);
    }
}

void onMouseClick(int button, int state, int x, int y) {
    if (state == GLUT_UP) return;
    switch (button) {
        case 3:
            if (fov < 179) fov++;
            break;
        case 4:
            if (fov > 1) fov--;
            break;
        default:
            break;
    }
}

/* DRAW FUNCTIONS */

void drawAxes() {
    // x axis
    glColor4f(RED);
    glBegin(GL_LINES);
    glVertex3i(0, 0, 0);
    glVertex3i(10, 0, 0);
    glEnd();
    // y axis
    glColor4f(GREEN);
    glBegin(GL_LINES);
    glVertex3i(0, 0, 0);
    glVertex3i(0, 10, 0);
    glEnd();
    // z axis
    glColor4f(BLUE);
    glBegin(GL_LINES);
    glVertex3i(0, 0, 0);
    glVertex3i(0, 0, 10);
    glEnd();
}

void drawText(const char *text, GLint x, GLint y, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    glColor4f(r, g, b, a);
    glRasterPos2i(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *) text);
}

void drawText(const char *text, GLint x, GLint y) {
    drawText(text, x, y, WHITE);
}

void display() {
    // fps calculation
    currentFrameTime = std::chrono::steady_clock::now();
    unsigned long delta = std::chrono::duration_cast<std::chrono::microseconds>(currentFrameTime - lastFrameTime).count();
    fps = 1000000.0 / delta;
    lastFrameTime = currentFrameTime;

    // observer changes
    if (animationPlaying) {
        animations[currentAnimation].tick(delta / 1000);
        if (animations[currentAnimation].isDone()) {
            animations[currentAnimation].reset();
            animationPlaying = false;
        }
    } else {
        observer.applyForce(
            (forwardKeyPressed - backwardKeyPressed) * (rightwardKeyPressed == leftwardKeyPressed ? 1 : M_SQRT1_2),
            (rightwardKeyPressed - leftwardKeyPressed) * (forwardKeyPressed == backwardKeyPressed ? 1 : M_SQRT1_2));
        observer.tick(delta / 1000);
    }

    // clear & set viewport
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, screenWidth, screenHeight);

    // enter 2D rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // draw debug info
    if (debugInfoOn) {
        std::ostringstream debugInfo;
        debugInfo
            << "FPS: " << std::fixed << std::setprecision(0) << fps << std::endl
            << "FOV: " << fov << std::endl
            << "animation: " << currentAnimation << std::endl
            << "x: " << std::setprecision(4) << observer.getPosition().x << " (" << std::showpos << observer.getVelocity().x << std::noshowpos << ")" << std::endl
            << "y: " << observer.getPosition().y << " (" << std::showpos << observer.getVelocity().y << std::noshowpos << ")" << std::endl
            << "z: " << observer.getPosition().z << " (" << std::showpos << observer.getVelocity().z << std::noshowpos << ")" << std::endl
            << "theta: " << observer.getAngle().theta << std::endl
            << "phi: " << observer.getAngle().phi << std::endl
            << "flashlight: " << flashlightOn << std::endl;
        drawText(debugInfo.str().c_str(), 10, screenHeight - 20);
    }

    // draw instructions
    if (instructionsOn) {
        std::ostringstream instructions;
        instructions
            << "Controls:" << std::endl
            << "[Move mouse] Look around" << std::endl
            << "[Scroll up] Increase FOV" << std::endl
            << "[Scroll down] Decrease FOV" << std::endl;
        for (const auto &key : keys) {
            instructions << "[" << key.getName() << "] " << key.getDescription() << std::endl;
        }
        drawText(instructions.str().c_str(), 10, 10 + 15.5217391304 * (4 + keys.size()));
    }

    // leave 2D rendering
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // enter 3D rendering
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float) screenWidth / screenHeight, 0.1, renderDistance);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw skybox
    if (skyboxOn) {
        glPushMatrix();
        gluLookAt(0, 0, 0, observer.getFrontVector().x, observer.getFrontVector().y, observer.getFrontVector().z, 0, 1, 0);
        glEnable(GL_TEXTURE_2D);
        glDepthMask(GL_FALSE);
        skybox->render();
        glDisable(GL_TEXTURE_2D);
        glDepthMask(GL_TRUE);
        glPopMatrix();
    }

    // set look at
    gluLookAt(
        observer.getPosition().x, observer.getPosition().y, observer.getPosition().z,
        observer.getFocusPoint().x, observer.getFocusPoint().y, observer.getFocusPoint().z,
        0, 1, 0);

    // draw axes
    if (axesOn) drawAxes();

    // change polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, wireframeOn ? GL_LINE : GL_FILL);

    // turn on scene features
    if (lightingOn) glEnable(GL_LIGHTING);
    if (!currentShader.empty()) shaders.at(currentShader).enable();
    if (cullingOn) glEnable(GL_CULL_FACE);

    // render lights
    for (const auto &light : lights) light->render();

    // render scene
    scene->render();

    // turn off scene features
    if (lightingOn) glDisable(GL_LIGHTING);
    if (!currentShader.empty()) Shader::clear();
    if (cullingOn) glDisable(GL_CULL_FACE);

    // swap buffers
    glutSwapBuffers();
}

/* TIMER FUNCTION */

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(0, timer, 1);
}

/* MAIN FUNCTION */

int main(int argc, char **argv) {
    // initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(300, 100);
    glutCreateWindow("uc2018280609@dei.uc.pt | Submarine Door");

    // initialize glew
    glewInit();

    // initialize assets
    initializeTextures();
    initializeLights();
    initializeShaders();
    initializeShapes();
    initializeAnimations();

    // set clear color as black
    glClearColor(BLACK);
    // enable depth
    glEnable(GL_DEPTH_TEST);
    // disable color interpolation
    glShadeModel(GL_FLAT);
    // normalize normals
    glEnable(GL_NORMALIZE);
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // display functions
    glutDisplayFunc(display);
    glutReshapeFunc([](int width, int height) {
        screenWidth = width;
        screenHeight = height;
        screenCenterX = screenWidth / 2;
        screenCenterY = screenHeight / 2;
    });

    // input functions
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onKeyUp);
    glutIgnoreKeyRepeat(1);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutPassiveMotionFunc(onMouseMove);
    glutMouseFunc(onMouseClick);

    // timer function
    glutTimerFunc(0, timer, 1);

    // start glut loop
    glutMainLoop();
}