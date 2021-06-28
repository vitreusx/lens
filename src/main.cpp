#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "window.h"
#include "shader.h"
#include "program.h"
#include "vao.h"
#include "buffer.h"
#include "camera.h"
#include "model.h"
#include "random.h"
#include "texture.h"
#include "storage_buffer.h"
#include "rayapx.h"
#include "ray.h"
#include "scene.h"

using namespace std;
using namespace glm;

class Base {
public:
    Window window;
    Scene scene;

    Camera camera;
    bool firstMouse = true;
    float priorX, priorY, priorTime;
    float dt;
    bool which = true, cursor = false, zone = false;

    bool createRay = false;

    static void onMouseMove(GLFWwindow *window, double x, double y) {
        Base *self = (Base*)glfwGetWindowUserPointer(window);
        if (self->cursor) return;

        if (self->firstMouse) {
            self->priorX = (float)x;
            self->priorY = (float)y;
            self->firstMouse = false;
        }

        float dx = (float)x - self->priorX, dy = (float)y - self->priorY;
        self->priorX = (float)x;
        self->priorY = (float)y;

        self->camera.onMouseMove(dx, -dy);
    }

    static void onMouseScroll(GLFWwindow *window, double dx, double dy) {
        Base *self = (Base*)glfwGetWindowUserPointer(window);
        if (self->cursor) return;

        self->camera.onMouseScroll((float)dy);
    }

    static void onKeyPress(GLFWwindow *window, int key, int, int action, int) {
        Base *self = (Base*)glfwGetWindowUserPointer(window);

        if (key == GLFW_KEY_F1 && action == GLFW_PRESS && !self->cursor) {
            self->which = !self->which;
        }

        if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
            self->cursor = !self->cursor;
            if (self->cursor) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                self->firstMouse = true;
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }

        if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
            self->zone = !self->zone;
        }
    }

    static void onMousePress(GLFWwindow *window, int button, int action, int) {
        Base *self = (Base*)glfwGetWindowUserPointer(window);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            auto& hole = self->scene.holes.front();
            vector<vec3> stars;
            for (const auto& star: self->scene.stars) {
                stars.push_back(star.pos);
            }

            self->scene.rays.emplace_back(self->camera.pos, self->camera.front,
                hole.pos, hole.r, 1000.0);
        }
    }

    void onInput() {
        if (cursor) return;

        static const vector<pair<int, Movement>> mvmts = {
            { GLFW_KEY_W, Forward },
            { GLFW_KEY_A, Left },
            { GLFW_KEY_S, Backward },
            { GLFW_KEY_D, Right },
            { GLFW_KEY_LEFT_SHIFT, Down },
            { GLFW_KEY_SPACE, Up }
        };

        float s = 10;
        static const vector<pair<int, vec2>> scrolls = {
            { GLFW_KEY_DOWN, vec2(0, -s) },
            { GLFW_KEY_UP, vec2(0, s) },
            { GLFW_KEY_LEFT, vec2(-s, 0) },
            { GLFW_KEY_RIGHT, vec2(s, 0) }
        };

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        s = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) ? 5 : 1;

        for (auto const& [key, mvmt]: mvmts) {
            if (glfwGetKey(window, key) == GLFW_PRESS)
                camera.onKeyPress(mvmt, s * dt);
        }

        for (auto const& [key, dv]: scrolls) {
            if (glfwGetKey(window, key) == GLFW_PRESS)
                camera.onMouseMove(s * dv.x, s * dv.y);
        }
    }

    void updateTime() {
        auto current = (float)glfwGetTime();
        dt = current - priorTime;
        priorTime = current;
    }

    void refresh() {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Base() {
        glfwSetWindowUserPointer(window, this);
        if (!cursor)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSetCursorPosCallback(window, onMouseMove);
        glfwSetScrollCallback(window, onMouseScroll);
        glfwSetKeyCallback(window, onKeyPress);
        glfwSetMouseButtonCallback(window, onMousePress);
    }
};

class NormalMode {
private:
    Base *base;
    Scene *scene;
    vec3 bgColor;

    Shader vs, fs;
    Program program;
    Model sphere;

public:
    explicit NormalMode(Base *base) {
        this->base = base;
        this->scene = &base->scene;
        vs = Shader("res/normal.vert", GL_VERTEX_SHADER);
        fs = Shader("res/normal.frag", GL_FRAGMENT_SHADER);
        program = Program({vs, fs});

        sphere = Model("res/sphere.obj");

        bgColor = vec3(0.1);
    }

    void render() {
        glUseProgram(program);

        program.set("view", base->camera.view());
        auto [w, h] = base->window.size();
        program.set("proj", base->camera.proj(w, h));

        glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto renderAll = [&](vector<Scene::Body> const& bodies) -> void {
            for (auto const& [pos, color, r]: bodies) {
                auto model = mat4(1);
                model = translate(model, pos);
                model = scale(model, vec3(r));
                program.set("model", model);
                program.set("color", color);
                sphere.render();
            }
        };

        renderAll(scene->holes);
        renderAll(scene->stars);

        for (auto& ray: scene->rays) {
            program.set("model", mat4(1));
            program.set("color", vec4(1, 1, 1, 1));
            ray.render();
        }
    }
};

class RaytracerMode {
private:
    Base *base;
    Scene *scene;

    Texture tex;
    ivec2 texSize;

    Model quad;

    Shader quadVs, quadFs, rayComp;
    Program quadProg, rayProg;
    StorageBuffer bodiesBuf, lowerPartBuf, upperPartBuf, colorBuf;

    vec3 bgColor;

    void loadDefl() {
        float lowCutoff = 2.6;
        float midCutoff = 3;
        float highCutoff = 10;
        int lowerRes = 500;
        int upperRes = 1000;

        rayProg.set("lowCutoff", lowCutoff);
        rayProg.set("lowerRes", lowerRes);
        rayProg.set("midCutoff", midCutoff);
        rayProg.set("upperRes", upperRes);
        rayProg.set("highCutoff", highCutoff);

        vector<float> lowerPart;
        for (int i = 0; i < lowerRes; ++i) {
            float b = lowCutoff + (float)i * (midCutoff - lowCutoff) / (float)lowerRes;
            lowerPart.push_back(grav::defl(grav::Rapprox(b)));
        }
        lowerPartBuf.load(lowerPart.data(), lowerPart.size() * sizeof(float));
        lowerPartBuf.bind(2);

        vector<float> upperPart;
        for (int i = 0; i < upperRes; ++i) {
            float b = midCutoff + (float)i * (highCutoff - midCutoff) / (float)upperRes;
            upperPart.push_back(grav::defl(grav::Rapprox(b)));
        }
        upperPartBuf.load(upperPart.data(), upperPart.size() * sizeof(float));
        upperPartBuf.bind(3);
    }

public:
    explicit RaytracerMode(Base *base) {
        this->base = base;
        this->scene = &base->scene;

        quadVs = Shader("res/quad.vert", GL_VERTEX_SHADER);
        quadFs = Shader("res/quad.frag", GL_FRAGMENT_SHADER);
        quadProg = Program({quadVs, quadFs});

        glUseProgram(quadProg);
        quadProg.set("rayTex", 0);

        rayComp = Shader("res/raytracer.comp", GL_COMPUTE_SHADER);
        rayProg = Program({rayComp});

        quad = Model("res/quad.obj");

        bgColor = vec3(0.1);
        glUseProgram(rayProg);
        rayProg.set("bgColor", bgColor);
        loadDefl();
    }

    void render() {
        glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto [w, h] = base->window.size();
        ivec2 extent(w, h);

        if (texSize != extent) {
            tex = Texture(w, h);
            texSize = extent;
        }

        glUseProgram(rayProg);
        tex.bindAsImage(0);

        rayProg.set("pos", base->camera.pos);
        rayProg.set("extent", extent);
        rayProg.set("zone", (int)base->zone);

        mat4 mv = base->camera.view(), proj = base->camera.proj(w, h);
        vec4 viewport(0, 0, w, h);

        vec3 rayLD = unProject(vec3(0, 0, -1), mv, proj, viewport);
        rayProg.set("rayLD", rayLD);

        vec3 rayLU = unProject(vec3(0, h, -1), mv, proj, viewport);
        rayProg.set("rayLU", rayLU);

        vec3 rayRU = unProject(vec3(w, h, -1), mv, proj, viewport);
        rayProg.set("rayRU", rayRU);

        vec3 rayRD = unProject(vec3(w, 0, -1), mv, proj, viewport);
        rayProg.set("rayRD", rayRD);

        int nstars = scene->stars.size();
        rayProg.set("nstars", nstars);

        int nholes = scene->holes.size();
        rayProg.set("nholes", nholes);

        bodiesBuf.load(scene->buffer.data(), scene->buffer.size() * sizeof(vec4));
        bodiesBuf.bind(1);

        colorBuf.load(scene->colorBuffer.data(), scene->colorBuffer.size() * sizeof
        (vec4));
        colorBuf.bind(4);

        glDispatchCompute(w / 8 + 1, h / 8 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glUseProgram(quadProg);
        tex.bindAsTex(0);
        quad.render();
    }
};

int main() {
    Base base;

    NormalMode normal(&base);
    RaytracerMode raytracer(&base);

    while (!glfwWindowShouldClose(base.window)) {
        base.updateTime();
        base.onInput();

        if (base.which) normal.render();
        else raytracer.render();

        base.refresh();
    }

    return 0;
}