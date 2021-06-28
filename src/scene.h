#pragma once
#include <glm/glm.hpp>
#include "random.h"
#include "ray.h"
#include <vector>
using namespace glm;

class Scene {
public:
    struct Body {
        vec3 pos;
        vec4 color;
        float r;
    };

    Random rand;
    vector<Ray> rays;
    vector<Body> holes, stars;
    vector<vec4> buffer;
    vector<vec4> colorBuffer;

    vec4 makeColor() {
        float r = rand.uniform(0.75, 1);
        float g = rand.uniform(0.75, 1);
        float b = rand.uniform(0.75, 1);
        return vec4(r, g, b, 1.0);
    }

    Scene() {
        int lim = -1;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                for (int k = -1; k <= 1; ++k) {
                    if (i != 0 || j != 0 || k != 0) {
                        Scene::Body body = {};
                        body.pos = { 10 * i, 10 * j, 10 * k };
                        body.r = rand.uniform(0.5, 1.5);
                        body.color = makeColor();
                        stars.push_back(body);
                    }
                }
            }
        }

        Scene::Body body = {};
        body.pos = { 0, 0, -100 };
        body.r = 0.5;
        body.color = vec4(0);
        holes.push_back(body);

        for (auto& star: stars) {
            buffer.emplace_back(star.pos.x, star.pos.y, star.pos.z, star.r);
            colorBuffer.emplace_back(star.color);
        }

        for (auto& hole: holes) {
            buffer.emplace_back(hole.pos.x, hole.pos.y, hole.pos.z, hole.r);
            colorBuffer.emplace_back(hole.color);
        }
    }
};