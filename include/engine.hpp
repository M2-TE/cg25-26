#pragma once
#include "mesh.hpp"
#include "input.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "transform.hpp"

struct Engine {
    Engine() {
        // create render components
        _window.init(1280, 720);
        _pipeline.init("default.vert", "default.frag");
        _texture.init("grass.png");
        _mesh.init();

        // move the camera to the back a little
        _camera._position.z = 5;
    }
    ~Engine() {
        // destroy in reversed init() order
        _mesh.destroy();
        _texture.destroy();
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
            glClearColor(0.01, 0.01, 0.01, 0.0);
        }
        // clear image before drawing to it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind graphics pipeline containing vertex and fragment shaders
        _pipeline.bind();

        // move via WASDQE
        float speed = 0.1;
        if (Keys::down(SDLK_W)) _camera._position.z -= speed;
        if (Keys::down(SDLK_A)) _camera._position.x -= speed;
        if (Keys::down(SDLK_S)) _camera._position.z += speed;
        if (Keys::down(SDLK_D)) _camera._position.x += speed;
        if (Keys::down(SDLK_Q)) _camera._position.y -= speed;
        if (Keys::down(SDLK_E)) _camera._position.y += speed;

        // let go of mouse capture when we press ESCAPE
        if (Mouse::captured() && Keys::pressed(SDLK_ESCAPE)) {
            Input::register_capture(false);
            SDL_SetWindowRelativeMouseMode(_window._window_p, Mouse::captured());
        }
        // grab mouse capture when we click into the window
        if (!Mouse::captured() && Mouse::pressed(Mouse::ids::left)) {
            Input::register_capture(true);
            SDL_SetWindowRelativeMouseMode(_window._window_p, Mouse::captured());
        }
        // camera rotation
        if (Mouse::captured()) {
            float mouse_sensitivity = 0.003f;
            _camera._rotation.x -= mouse_sensitivity * Mouse::delta().second;
            _camera._rotation.y -= mouse_sensitivity * Mouse::delta().first;
        }

        // bind camera to the pipeline
        _camera.bind();

        // bind and draw mesh
        // _transform._rotation += 0.5 * _time._delta;
        _transform.bind();
        _texture.bind();
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
    Camera _camera;
    Texture _texture;
    Pipeline _pipeline;
    Transform _transform;
};
