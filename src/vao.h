#pragma once
#include <glad/glad.h>

class VAO {
private:
    GLuint vao = 0;

public:
    VAO() {
        glGenVertexArrays(1, &vao);
    }

    ~VAO() {
        glDeleteVertexArrays(1, &vao);
    }

    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    VAO(VAO&& other) {
        *this = move(other);
    }

    VAO& operator=(VAO&& other) {
        glDeleteVertexArrays(1, &vao);
        vao = other.vao;
        other.vao = 0;
        return *this;
    }

    operator GLuint&() {
        return vao;
    }
};
