#pragma once
#include "window.hpp"

struct Engine {
    Engine() {
        _window.init();
    }
    ~Engine() {
        _window.destroy();
    }

    auto handle_sdl_event(SDL_Event& event) -> SDL_AppResult {
        switch (event.type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            case SDL_EventType::SDL_EVENT_KEY_UP: /*event.key.key*/ break;
            case SDL_EventType::SDL_EVENT_KEY_DOWN: /*event.key.key*/ break;
            default: break;
        }
        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    auto handle_sdl_frame() -> SDL_AppResult {
        struct Vertex { // sizeof(Vertex) = sizeof(float) * 4 = 16 bytes
            float x; float y; float z; float w;
        };
        std::vector<Vertex> vertices = {
            { +0.0, +0.5, +0.0, +1.0 }, // top
            { +0.5, -0.5, +0.0, +1.0 }, // bottom right
            { -0.5, -0.5, +0.0, +1.0 }, // bottom left
        };
        // create GPU buffer to store vertices
        GLuint buffer_vertex;
        glCreateBuffers(1, &buffer_vertex);
        // upload to GPU
        glNamedBufferStorage(buffer_vertex, 
            vertices.size() * sizeof(Vertex), // number of bytes to upload
            vertices.data(), // raw data to upload
            BufferStorageMask::GL_NONE_BIT);
        
        // create index buffer the same way
        std::vector<GLuint> indices = { 0, 1, 2 };
        GLuint buffer_index; // element = index
        glCreateBuffers(1, &buffer_index);
        glNamedBufferStorage(buffer_index,
            indices.size() * sizeof(GLuint),
            indices.data(),
            BufferStorageMask::GL_NONE_BIT);

        // create vertex array (basically a mesh)
        GLuint buffer_mesh;
        glCreateVertexArrays(1, &buffer_mesh);
        // assign vertex and index buffer
        glVertexArrayVertexBuffer(buffer_mesh, 0, buffer_vertex, 0, sizeof(Vertex));
        glVertexArrayElementBuffer(buffer_mesh, buffer_index);
        // describe the data inside each Vertex
        glVertexArrayAttribFormat(buffer_mesh, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(buffer_mesh, 0, 0);
        glEnableVertexArrayAttrib(buffer_mesh, 0);

        // compile vertex shader
        const GLchar* vs_string = "\n\
        #version 460 core\n\
        layout(location = 0) in vec4 in_pos;\n\
        void main() {\n\
            gl_Position = in_pos;\n\
        }";
        GLuint shader_vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader_vertex, 1, &vs_string, nullptr);
        glCompileShader(shader_vertex);
        // check compilation results
        GLint success;
        std::vector<GLchar> info_log(512);
        glGetShaderiv(shader_vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_vertex, info_log.size(), nullptr, info_log.data());
            std::println("{}", info_log.data());
        }

        // compile fragment shader
        const GLchar* fs_string = "\n\
        #version 460 core\n\
        out vec4 out_color;\n\
        void main() {\n\
            out_color = vec4(1.0, 0.0, 0.0, 1.0);\n\
        }";
        GLuint shader_fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader_fragment, 1, &fs_string, nullptr);
        glCompileShader(shader_fragment);
        // check compilation results
        glGetShaderiv(shader_fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader_fragment, info_log.size(), nullptr, info_log.data());
            std::println("{}", info_log.data());
        }
        // create graphics pipeline (aka program)
        GLuint shader_program = glCreateProgram();
        glAttachShader(shader_program, shader_vertex);
        glAttachShader(shader_program, shader_fragment);
        glLinkProgram(shader_program);
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader_program, info_log.size(), nullptr, info_log.data());
            std::println("{}", info_log.data());
        }
        // clean up shader sources after compilation
        glDeleteShader(shader_vertex);
        glDeleteShader(shader_fragment);

        /// now we can actually draw things
        // clear image before drawing to it
        glClearColor(0.5, 0.5, 0.5, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        // bind graphics pipeline
        glUseProgram(shader_program);
        // bind mesh
        glBindVertexArray(buffer_mesh);
        // finally, draw mesh
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
        // present drawn image to screen
        SDL_GL_SwapWindow(_window._window_p);

        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    Window _window;
};