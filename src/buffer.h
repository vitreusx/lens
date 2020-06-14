#pragma once
#include <glad/glad.h>

class Buffer {
private:
    GLuint buffer = 0;

public:
    Buffer() {
        glGenBuffers(1, &buffer);
    }

    ~Buffer() {
        glDeleteBuffers(1, &buffer);
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) {
        *this = move(other);
    }

    Buffer& operator=(Buffer&& other) {
        glDeleteBuffers(1, &buffer);
        buffer = other.buffer;
        other.buffer = 0;
        return *this;
    }

    operator GLuint&() {
        return buffer;
    }
};