#include <GL/glew.h>

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

#include "structures.hpp"

using UniformType = std::variant<DynamicValue<int>, DynamicValue<float>, DynamicValue<std::vector<int>>>;

class UniformVariable {
    private:
    UniformType value;
    int location;

    public:
    UniformVariable(DynamicValue<int> value, int location) : value(value), location(location) {}
    UniformVariable(DynamicValue<float> value, int location) : value(value), location(location) {}
    UniformVariable(DynamicValue<std::vector<int>> value, int location) : value(value), location(location) {}

    void upload() {
        std::visit([this](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, DynamicValue<int>>) {
                glUniform1i(location, value());
            } else if constexpr (std::is_same_v<T, DynamicValue<float>>) {
                glUniform1f(location, value());
            } else if constexpr (std::is_same_v<T, DynamicValue<std::vector<int>>>) {
                std::vector<int> array = value();
                glUniform1iv(location, array.size(), &array[0]);
            }
        },
                   value);
    }
};

class Shader {
    private:
    GLuint vertId, fragId, id;
    std::vector<UniformVariable> uniforms;

    void log(std::string name, GLuint id) {
        GLint logLength;
        std::vector<GLchar> log;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        log.resize(logLength);
        glGetShaderInfoLog(id, logLength, &logLength, &log[0]);
        std::cout << name << " compile time errors:\n"
                  << std::string(log.begin(), log.end()) << std::endl;
    }

    public:
    Shader(std::string vertSrc, std::string fragSrc)
        : Shader(vertSrc, fragSrc, {}) {}
    Shader(std::string vertSrc, std::string fragSrc, std::map<std::string, UniformType> uniforms) {
        // create shaders
        vertId = glCreateShader(GL_VERTEX_SHADER);
        fragId = glCreateShader(GL_FRAGMENT_SHADER);
        // set shaders source code
        const char *vertSrcC, *fragSrcC;
        glShaderSource(vertId, 1, &(vertSrcC = vertSrc.c_str()), NULL);
        glShaderSource(fragId, 1, &(fragSrcC = fragSrc.c_str()), NULL);
        // compile shaders
        glCompileShaderARB(vertId);
        glCompileShaderARB(fragId);
        // if there are compile errors, log & throw
        GLint vertCompiled, fragCompiled;
        glGetShaderiv(vertId, GL_COMPILE_STATUS, &vertCompiled);
        glGetShaderiv(fragId, GL_COMPILE_STATUS, &fragCompiled);
        if (!vertCompiled) log("Vertex Shader", vertId);
        if (!fragCompiled) log("Fragment Shader", fragId);
        if (!vertCompiled || !fragCompiled) throw;
        // create shader program
        id = glCreateProgramObjectARB();
        glAttachShader(id, vertId);
        glAttachShader(id, fragId);
        glLinkProgram(id);
        // populate uniforms vector
        for (const auto& [name, variant] : uniforms) {
            GLint location = glGetUniformLocation(id, name.c_str());
            this->uniforms.push_back(std::visit([&location](auto&& value) { return UniformVariable(value, location); }, variant));
        }
    }

    ~Shader() {
        glDetachShader(id, vertId);
        glDetachShader(id, fragId);
        glDeleteShader(id);
    }

    void enable() {
        glUseProgramObjectARB(id);
        for (auto uniform : uniforms) {
            uniform.upload();
        }
    }

    static void clear() { glUseProgramObjectARB(0); }
};