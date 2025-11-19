#pragma once

// sizeof(Vertex) = sizeof(float) * 4 = 16 bytes
struct Vertex {
    glm::vec4 position; // x, y, z, w
    glm::vec4 color; // r, g, b, a
    glm::vec2 uv; // u, v coordinates for textures
};
// sizeof(Index) = 4 bytes
using Index = GLuint;

struct Mesh {
    void init() {
        float p = +0.5f; // for readability
        float n = -0.5f; // for readability
        std::vector<Vertex> vertices = {
            Vertex{ glm::vec4{n, n, p, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.33, 0.75} }, // front
            Vertex{ glm::vec4{p, n, p, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.66, 0.75} },
            Vertex{ glm::vec4{n, p, p, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.33, 0.50} },
            Vertex{ glm::vec4{p, p, p, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.66, 0.50} },
            Vertex{ glm::vec4{n, n, n, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.33, 0.00} }, // back
            Vertex{ glm::vec4{p, n, n, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.66, 0.00} },
            Vertex{ glm::vec4{n, p, n, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.33, 0.25} },
            Vertex{ glm::vec4{p, p, n, 1}, glm::vec4{1, 0, 0, 1}, glm::vec2{0.66, 0.25} },
            Vertex{ glm::vec4{n, n, n, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.00, 0.50} }, // left
            Vertex{ glm::vec4{n, n, p, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.00, 0.25} },
            Vertex{ glm::vec4{n, p, n, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.33, 0.50} },
            Vertex{ glm::vec4{n, p, p, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.33, 0.25} },
            Vertex{ glm::vec4{p, n, n, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{1.00, 0.50} }, // right
            Vertex{ glm::vec4{p, n, p, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{1.00, 0.25} },
            Vertex{ glm::vec4{p, p, n, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.66, 0.50} },
            Vertex{ glm::vec4{p, p, p, 1}, glm::vec4{0, 1, 0, 1}, glm::vec2{0.66, 0.25} },
            Vertex{ glm::vec4{n, p, n, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.33, 0.25} }, // top
            Vertex{ glm::vec4{n, p, p, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.33, 0.50} },
            Vertex{ glm::vec4{p, p, n, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.66, 0.25} },
            Vertex{ glm::vec4{p, p, p, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.66, 0.50} },
            Vertex{ glm::vec4{n, n, n, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.33, 0.75} }, // bottom
            Vertex{ glm::vec4{n, n, p, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.33, 1.00} },
            Vertex{ glm::vec4{p, n, n, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.66, 0.75} },
            Vertex{ glm::vec4{p, n, p, 1}, glm::vec4{0, 0, 1, 1}, glm::vec2{0.66, 1.00} },
        };
        std::vector<Index> indices = { 
             0,  1,  3,  3,  2,  0, // front
             5,  4,  7,  7,  4,  6, // back
             8,  9, 11, 11, 10,  8, // left
            13, 12, 15, 15, 12, 14, // right
            16, 17, 19, 19, 18, 16, // top
            23, 21, 20, 23, 20, 22, // bottom
        };
        _index_count = indices.size();

        // create GPU buffer to store vertices
        glCreateBuffers(1, &_buffer_vertices);
        // upload to GPU
        glNamedBufferStorage(_buffer_vertices, 
            vertices.size() * sizeof(Vertex), // number of bytes to upload
            vertices.data(), // raw data to upload
            BufferStorageMask::GL_NONE_BIT);
        
        // create index buffer the same way
        glCreateBuffers(1, &_buffer_indices);
        // upload to GPU
        glNamedBufferStorage(_buffer_indices,
            indices.size() * sizeof(Index),
            indices.data(),
            BufferStorageMask::GL_NONE_BIT);

        // create vertex array (basically a mesh)
        glCreateVertexArrays(1, &_buffer_mesh);
        // assign vertex and index buffer
        glVertexArrayVertexBuffer(_buffer_mesh, 0, _buffer_vertices, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(_buffer_mesh, _buffer_indices);

        // describe the data inside each Vertex
        // Vertex::position
        glVertexArrayAttribFormat(_buffer_mesh, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(_buffer_mesh, 0, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 0);
        // Vertex::color
        glVertexArrayAttribFormat(_buffer_mesh, 1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex::position));
        glVertexArrayAttribBinding(_buffer_mesh, 1, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 1);
        // Vertex::uv
        glVertexArrayAttribFormat(_buffer_mesh, 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::position) + sizeof(Vertex::color));
        glVertexArrayAttribBinding(_buffer_mesh, 2, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 2);
    }
    void destroy() {
        glDeleteBuffers(1, &_buffer_indices);
        glDeleteBuffers(1, &_buffer_vertices);
        glDeleteVertexArrays(1, &_buffer_mesh);
    }

    void bind() {
        glBindVertexArray(_buffer_mesh);
    }
    void draw() {
        glDrawElements(GL_TRIANGLES, _index_count, GL_UNSIGNED_INT, nullptr);
    }

    GLuint _buffer_vertices;
    GLuint _buffer_indices;
    GLuint _buffer_mesh;
    GLuint _index_count;
};