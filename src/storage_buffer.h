#pragma once
#include <glad/glad.h>
#include <utility>
using namespace std;

class StorageBuffer {
private:
    GLuint buffer = 0;

public:
    StorageBuffer() {
        glGenBuffers(1, &buffer);
    }

    ~StorageBuffer() {
        glDeleteBuffers(1, &buffer);
    }

    void load(void *data, size_t nbytes) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, nbytes, data, GL_DYNAMIC_COPY);
    }

    void bind(int n) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, n, buffer);
    }

    StorageBuffer(const StorageBuffer&) = delete;
    StorageBuffer& operator=(const StorageBuffer&) = delete;

    StorageBuffer(StorageBuffer&& other) {
        *this = move(other);
    }

    StorageBuffer& operator=(StorageBuffer&& other) {
        glDeleteBuffers(1, &buffer);
        buffer = other.buffer;
        other.buffer = 0;
        return *this;
    }

    operator GLuint&() {
        return buffer;
    }
};