#pragma once
#include <glad/glad.h>
#include <utility>
using namespace std;

class Texture {
private:
    GLuint tex;

public:
    Texture() = default;

    Texture(int w, int h) {
        glGenTextures(1, &tex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    void bindAsImage(int num) {
        glBindImageTexture(num, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    }

    void bindAsTex(int num) {
        glActiveTexture(GL_TEXTURE0 + num);
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    ~Texture() {
        glDeleteTextures(1, &tex);
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) {
        *this = move(other);
    }

    Texture& operator=(Texture&& other) {
        glDeleteTextures(1, &tex);
        tex = other.tex;
        other.tex = 0;
        return *this;
    }

    operator GLuint&() {
        return tex;
    }
};