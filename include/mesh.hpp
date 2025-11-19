#pragma once

// sizeof(Vertex) = sizeof(float) * 4 = 16 bytes
struct Vertex {
    float x;
    float y;
    float z;
    float w;
};
// sizeof(Index) = 4 bytes
using Index = GLuint;

struct Mesh {
    void init() {
        std::vector<Vertex> vertices = {
            { +0.0, +0.5, +0.0, +1.0 }, // top
            { +0.5, -0.5, +0.0, +1.0 }, // bottom right
            { -0.5, -0.5, +0.0, +1.0 }, // bottom left
        };
        std::vector<Index> indices = { 0, 1, 2 };
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
        glVertexArrayAttribFormat(_buffer_mesh, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(_buffer_mesh, 0, 0);
        glEnableVertexArrayAttrib(_buffer_mesh, 0);
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