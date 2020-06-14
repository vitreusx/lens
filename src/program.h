#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <stdexcept>
using namespace std;
using namespace glm;

class Program {
private:
    GLuint program = 0;
    unordered_map<string, GLint> locs = {};

    GLint retrieveLoc(const char* var) {
        GLint loc = 0;
        decltype(locs)::iterator it;
        if ((it = locs.find((string)var)) != locs.end()) {
            loc = it->second;
        }
        else {
            loc = glGetUniformLocation(program, var);
            locs[var] = loc;
        }

        return loc;
    }

public:
    Program() = default;

    explicit Program(const vector<GLuint>& shaders) {
        program = glCreateProgram();
        locs = {};

        for (const auto& shader: shaders)
            glAttachShader(program, shader);

        glLinkProgram(program);
        for (const auto& shader: shaders)
            glDetachShader(program, shader);

        GLint rv;
        glGetProgramiv(program, GL_LINK_STATUS, &rv);
        if (!rv) {
            GLint len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

            string log;
            log.resize(len);
            glGetProgramInfoLog(program, len, nullptr, log.data());

            throw runtime_error(log);
        }
    }

    void set(const char* var, mat4 const& val) {
        glUniformMatrix4fv(retrieveLoc(var), 1, GL_FALSE, value_ptr(val));
    }

    void set(const char *var, vec3 const& val) {
        glUniform3fv(retrieveLoc(var), 1, value_ptr(val));
    }

    void set(const char *var, int const& val) {
        glUniform1i(retrieveLoc(var), val);
    }

    Program(const Program&) = delete;
    Program& operator=(const Program&) = delete;

    Program(Program&& other) {
        *this = move(other);
    }

    Program& operator=(Program&& other) {
        glDeleteProgram(program);
        program = other.program;
        other.program = 0;
        return *this;
    }

    ~Program() {
        glDeleteProgram(program);
    }

    operator GLuint&() {
        return program;
    }
};