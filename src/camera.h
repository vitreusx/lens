#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

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
        return perspective(radians(zoom), (float)w / (float)h, 0.1f, 1000.0f);
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
        yaw += dx * sensitivity * (zoom / 45);
        pitch += dy * sensitivity * (zoom / 45);

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
