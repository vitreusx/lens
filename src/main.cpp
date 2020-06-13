#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <unordered_map>

using namespace std;
using namespace glm;

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

        glEnable(GL_DEPTH_TEST);
    }

    ~Window() {
        glfwTerminate();
    }

    operator GLFWwindow*() {
        return window;
    }

    pair<int, int> size() {
        pair<int, int> rv;
        glfwGetWindowSize(window, &rv.first, &rv.second);
        return rv;
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
    unordered_map<string, GLint> locs = {};

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
        GLint loc = 0;
        decltype(locs)::iterator it;
        if ((it = locs.find((string)var)) != locs.end()) {
            loc = it->second;
        }
        else {
            loc = glGetUniformLocation(program, var);
            locs[var] = loc;
        }

        glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(val));
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
            sizeof(Vertex), (void*)offsetof(Vertex, r));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), (void*)offsetof(Vertex, c));
        glEnableVertexAttribArray(1);
    }
};

enum Movement {
    Forward, Backward, Left, Right, Up, Down
};

class Camera {
private:
    vec3 pos, front, up, right, worldUp;
    float yaw, pitch, speed, sensitivity, zoom;

    void update() {
        vec3 f;
        f.x = cos(radians(yaw)) * cos(radians(pitch));
        f.y = sin(radians(pitch));
        f.z = sin(radians(yaw)) * cos(radians(pitch));

        front = normalize(f);
        right = normalize(cross(front, worldUp));
        up = normalize(cross(right, front));
    }

public:
    Camera() {
        pos = vec3(0, 0, 0);
        front = vec3(0, 0, -1);
        worldUp = up = vec3(0, 1, 0);

        speed = 2.5;
        yaw = -90;
        pitch = 0;
        sensitivity = 0.1;
        zoom = 45;

        update();
    }

    mat4 proj(int w, int h) const {
        return perspective(radians(zoom), (float)w / (float)h, 0.1f, 100.0f);
    }

    mat4 view() const {
        return lookAt(pos, pos + front, up);
    }

    void onKeyPress(Movement mvmt, float dt) {
        auto dist = speed * dt;
        switch (mvmt) {
            case Forward: pos += front * dist; break;
            case Backward: pos -= front * dist; break;
            case Left: pos -= right * dist; break;
            case Right: pos += right * dist; break;
            case Up: pos += up * dist; break;
            case Down: pos -= up * dist; break;
        }

        update();
    }

    void onMouseMove(float dx, float dy) {
        yaw += dx * sensitivity;
        pitch += dy * sensitivity;

        if (pitch > 89) pitch = 89;
        else if (pitch < -89) pitch = -89;

        update();
    }

    void onMouseScroll(float dy) {
        zoom -= dy;
        if (zoom < 1) zoom = 1;
        else if (zoom > 45) zoom = 45;

        update();
    }
};

class VBO {
private:
    GLuint vbo;
};

Camera camera;
bool firstMouse = true;
float priorX, priorY;

int main() {
    Window window;
    glViewport(0, 0, 800, 600);

    Shader vs(contents("shader.vert").c_str(), GL_VERTEX_SHADER);
    Shader fs(contents("shader.frag").c_str(), GL_FRAGMENT_SHADER);
    Program program({ vs, fs });

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

    glUseProgram(program);

    float priorT = 0, dt = 0;
    vector<pair<int, Movement>> mvmts = {
        { GLFW_KEY_W, Forward },
        { GLFW_KEY_A, Left },
        { GLFW_KEY_S, Backward },
        { GLFW_KEY_D, Right },
        { GLFW_KEY_LEFT_SHIFT, Down },
        { GLFW_KEY_SPACE, Up }
    };

    float s = 10;
    vector<pair<int, vec2>> scrolls = {
        { GLFW_KEY_DOWN, vec2(0, -s) },
        { GLFW_KEY_UP, vec2(0, s) },
        { GLFW_KEY_LEFT, vec2(-s, 0) },
        { GLFW_KEY_RIGHT, vec2(s, 0) }
    };

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, [](GLFWwindow*, double x, double y) -> void {
        if (firstMouse) {
            priorX = (float)x;
            priorY = (float)y;
            firstMouse = false;
        }

        float dx = (float)x - priorX, dy = (float)y - priorY;
        priorX = (float)x;
        priorY = (float)y;

        camera.onMouseMove(dx, -dy);
    });

    glfwSetScrollCallback(window, [](GLFWwindow*, double dx, double dy) -> void {
        camera.onMouseScroll((float)dy);
    });

    while (!glfwWindowShouldClose(window)) {
        auto current = (float)glfwGetTime();
        dt = current - priorT;
        priorT = current;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        for (auto const& [key, mvmt]: mvmts) {
            if (glfwGetKey(window, key) == GLFW_PRESS)
                camera.onKeyPress(mvmt, dt);
        }

        for (auto const& [key, dv]: scrolls) {
            if (glfwGetKey(window, key) == GLFW_PRESS)
                camera.onMouseMove(dv.x, dv.y);
        }

        program.set("model", mat4(1));
        program.set("view", camera.view());
        auto [w, h] = window.size();
        program.set("proj", camera.proj(w, h));

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}