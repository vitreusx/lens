#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
using namespace std;

class Window {
private:
    GLFWwindow *window;

    static void resetViewport(GLFWwindow*, int w, int h) {
        glViewport(0, 0, w, h);
    }

public:
    Window() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(800, 600, "lens", nullptr, nullptr);
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, resetViewport);

        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    ~Window() {
        glfwTerminate();
    }

    operator GLFWwindow*() {
        return window;
    }
};

class Shader {
private:
    GLuint shader = 0;

public:
    Shader() = default;

    Shader(const char *source, int type) {
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);

        glCompileShader(shader);
        GLint rv;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &rv);

        if (!rv) {
            GLint len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            string log;
            log.resize(len);
            glGetShaderInfoLog(shader, len, nullptr, log.data());

            throw runtime_error(log);
        }
    }

    ~Shader() {
        glDeleteShader(shader);
    }

    operator GLuint&() {
        return shader;
    }
};

class Program {
private:
    GLuint program = 0;

public:
    Program() = default;

    Program(const vector<GLuint>& shaders) {
        program = glCreateProgram();

        for (const auto& shader: shaders)
            glAttachShader(program, shader);

        glLinkProgram(program);
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

    ~Program() {
        glDeleteProgram(program);
    }

    operator GLuint&() {
        return program;
    }
};

string contents(const char *filename) {
    ifstream ifs(filename);
    string rv;

    ifs.seekg(0, ios::end);
    auto nbytes = ifs.tellg();
    rv.reserve(nbytes);
    ifs.seekg(0, ios::beg);

    ifs.read(rv.data(), nbytes);
    return rv;
}

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

    operator GLuint&() {
        return vao;
    }
};

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

    operator GLuint&() {
        return buffer;
    }
};

struct Vertex {
    float r[3], c[3];

    static void attr() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            sizeof(r), (void*)offsetof(Vertex, r));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            sizeof(c), (void*)offsetof(Vertex, c));
        glEnableVertexAttribArray(1);
    }
};

class VBO {
private:
    GLuint vbo;
};

int main() {
    Window window;
    glViewport(0, 0, 800, 600);

    Program prog; {
        Shader vs(contents("shader.vert").c_str(), GL_VERTEX_SHADER);
        Shader fs(contents("shader.frag").c_str(), GL_FRAGMENT_SHADER);
        prog = Program({ vs, fs });
    }

    vector<Vertex> verts = {
        {{0.5, 0.5, 0.0}, {1.0, 0.0, 0.0}},
        {{0.5, -0.5, 0.0}, {0.0, 1.0, 0.0}},
        {{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}},
        {{ -0.5, 0.5, 0.0}, { 1.0, 1.0, 1.0}}
    };
    vector<GLuint> indices = {
        0, 1, 3,
        1, 2, 3
    };

    VAO vao;
    glBindVertexArray(vao);

    Buffer vbo, ebo;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
        indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex),
        verts.data(), GL_STATIC_DRAW);

    Vertex::attr();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(prog);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}