#include <mesh>
#include <print>
#include <ui_render>
#include <ui_component>
#include <ka_font>
#include <glm/gtc/matrix_transform.hpp>
namespace Kawai
{

    void Mesh::init(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), this->vertices.data(), GL_DYNAMIC_DRAW);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), this->indices.data(), GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoord)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, color)));
        glBindVertexArray(0);
    }

    Mesh CreateUIRectMesh(float x, float y, float w, float h)
    {
        std::vector<Vertex> vertices{
            {Vertex{.position = glm::vec2(x, y),
                    .texCoord = {0.0f, 0.0f},
                    .color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)}},
            {Vertex{.position = glm::vec2(x, y + h), .texCoord = {0.0f, 1.0f}, .color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)}},
            {Vertex{.position = glm::vec2(x + w, y + h), .texCoord = {1.0f, 1.0f}, .color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)}},
            {Vertex{.position = glm::vec2(x + w, y), .texCoord = {1.0f, 0.0f}, .color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)}}};
        std::vector<GLuint> indices{
            0, 1, 2, 0, 2, 3};
        return Mesh(vertices, indices);
    }

    Mesh CreateUnitQuad()
    {
        std::vector<Vertex> vertices = {
            {{0.0f, 0.0f}, {0.0f, 0.0f}, {1, 1, 1, 1}},
            {{0.0f, 1.0f}, {0.0f, 1.0f}, {1, 1, 1, 1}},
            {{1.0f, 1.0f}, {1.0f, 1.0f}, {1, 1, 1, 1}},
            {{1.0f, 0.0f}, {1.0f, 0.0f}, {1, 1, 1, 1}}};

        std::vector<GLuint> indices = {
            0, 1, 2,
            0, 2, 3};

        return Mesh(vertices, indices);
    }

    void ModifyUIRectMesh(Mesh &mesh, float x, float y, float w, float h)
    {
        // std::println("({}, {}, {}, {})", x, y, w, h);
        auto &v = mesh.vertices;
        v[0].position = {x, y};
        v[1].position = {x, y + h};
        v[2].position = {x + w, y + h};
        v[3].position = {x + w, y};
    }

}