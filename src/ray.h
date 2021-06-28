#pragma once
#include <Eigen/Eigen>
#include <vector>
#include <cmath>
#include "vao.h"
#include "buffer.h"
using namespace Eigen;

class Ray {
private:
    VAO vao;
    Buffer vertBuf;
    int nverts;

    vector<Vertex> makePath(Vector3d const& p, Vector3d const& r, Vector3d h,
            double Rs, double far) {
        Vector3d d = h - p;
        Vector3d k = r.cross(d).normalized();
        Vector3d ray = -d.normalized();
        double R = d.norm() / Rs;

        double u = 1.0 / R;
        double cosAlpha = d.dot(r) / d.norm();
        double sinAlpha = sqrt(1.0 - cosAlpha * cosAlpha);
        double b = (R * sinAlpha) / sqrt(1.0 - 1.0 / R);
        double du2 = 1.0 / (b * b) - (1.0 - u) * u * u;
        Vector2d f(u, sqrt(du2));

        auto df = [&](Vector2d f) -> Vector2d {
            double du = f.y();
            double d2u = - f.x() * (1.0 - 1.5 * f.x());
            return Vector2d(du, d2u);
        };

        double phi = 0;
        Vector2d k1, k2, k3, k4;
        vector<Vertex> verts = {};
        while (true) {
            double rad = Rs * 1.0 / f.x();
            if (rad <= Rs || rad > far) break;

            Vector3d vrot = ray * cos(phi) +
                            k.cross(ray) * sin(phi) +
                            k * k.dot(ray) * (1.0 - cos(phi));
            Vector3d pos = h + rad * vrot;

            Vertex vert;
            vert.Position.X = pos.x();
            vert.Position.Y = pos.y();
            vert.Position.Z = pos.z();
            verts.push_back(vert);

            for (double delta = 0.01; ; delta /= 2.0) {
                k1 = df(f);
                k2 = df(f + (delta / 2.0) * k1);
                k3 = df(f + (delta / 2.0) * k2);
                k4 = df(f + delta * k3);
                auto newF = f + (delta / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
                auto diff = newF - f;
                if (diff.x() < 0.1) {
                    f = newF;
                    phi += delta;
                    break;
                }
            }
        }

        return verts;
    }

public:
    Ray() = default;

    Ray(vec3 p, vec3 r, vec3 h, double Rs, double far) {
        auto verts = makePath(Vector3d(p.x, p.y, p.z),
            Vector3d(r.x, r.y, r.z).normalized(),
            Vector3d(h.x, h.y, h.z), Rs, far);

        nverts = verts.size();
        glBindVertexArray(vao);

        if (nverts > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex),
                         verts.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), (void*)offsetof(Vertex, Position));
            glEnableVertexAttribArray(0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void render() {
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, nverts);
    }
};