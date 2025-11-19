#pragma once
#include "mesh.hpp"
#include "input.hpp"
#include "window.hpp"
#include "pipeline.hpp"
#include "transform.hpp"

struct Engine {
    Engine() {
        _window.init();
        _pipeline.init("default.vert", "default.frag");
        _mesh.init();
    }
    ~Engine() {
        // destroy in reversed init() order
        _mesh.destroy();
        _pipeline.destroy();
        _window.destroy();
    }

    auto handle_sdl_event(SDL_Event& event) -> SDL_AppResult {
        switch (event.type) {
            case SDL_EventType::SDL_EVENT_QUIT: return SDL_AppResult::SDL_APP_SUCCESS;
            default: break;
        }
        Input::register_event(event);
        return SDL_AppResult::SDL_APP_CONTINUE;
    }
    auto handle_sdl_frame() -> SDL_AppResult {
        // choose color to clear screen with
        if (Keys::down(SDLK_F)) {
            glClearColor(1.0, 0.5, 0.5, 0.0);
        }
        else {
            glClearColor(0.5, 0.5, 0.5, 0.0);
        }
        // clear image before drawing to it
        glClear(ClearBufferMask::GL_COLOR_BUFFER_BIT);

        // bind graphics pipeline containing vertex and fragment shaders
        _pipeline.bind();

        // bind and draw all the meshes we want
        // move via WASD
        if (Keys::down(SDLK_W)) _transform._position.y += 0.01;
        if (Keys::down(SDLK_A)) _transform._position.x -= 0.01;
        if (Keys::down(SDLK_S)) _transform._position.y -= 0.01;
        if (Keys::down(SDLK_D)) _transform._position.x += 0.01;
        // rotate via QE
        if (Keys::down(SDLK_Q)) _transform._rotation.z -= 0.01;
        if (Keys::down(SDLK_E)) _transform._rotation.z += 0.01;
        // scale via mouse buttons
        if (Mouse::down(Mouse::ids::left)) _transform._scale += 0.01;
        if (Mouse::down(Mouse::ids::right)) _transform._scale -= 0.01;
        _transform.bind();
        _mesh.bind();
        _mesh.draw();

        // present drawn image to screen
        SDL_GL_SwapWindow(_window._window_p);
        // clear single-frame inputs
        Input::flush();
        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    Mesh _mesh;
    Window _window;
    Pipeline _pipeline;
    Transform _transform;
};