#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <OBJ_Loader.h>
#include <stdexcept>
using namespace objl;
using namespace std;

class Model {
private:
    VAO vao;
    Buffer vertBuf, idxBuf;
    GLuint nfaces = 0;

    static void attr() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
            sizeof(Vertex), (void*)offsetof(Vertex, TextureCoordinate));
    }

public:
    Model() = default;

    Model(const char *filename) {
        auto loader = Loader();
        if (!loader.LoadFile(filename) || loader.LoadedMeshes.empty())
            throw runtime_error("Failed to load file.");

        auto& mesh = loader.LoadedMeshes[0];
        auto& vertices = mesh.Vertices;
        auto& faces = mesh.Indices;

        glBindVertexArray(vao);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint),
                     faces.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     vertices.data(), GL_STATIC_DRAW);

        attr();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        nfaces = faces.size();
    }

    void render() {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxBuf);
        glDrawElements(GL_TRIANGLES, nfaces, GL_UNSIGNED_INT, nullptr);
    }
};
