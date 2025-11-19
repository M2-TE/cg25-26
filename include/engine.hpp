#pragma once
#include "time.hpp"
#include "mesh.hpp"
#include "input.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "pipeline.hpp"
#include "transform.hpp"

struct Engine {
    Engine() {
        _time.init();

        // create render components
        _window.init(1280, 720);
        _pipeline_textured.init("default.vert", "textured.frag");
        _pipeline_vertcols.init("default.vert", "vertcols.frag");
        _texture.init("grass.png");
        _mesh.init();

        // move the camera to the back a little
        _camera._position.z = 5;
    }
    ~Engine() {
        // destroy in reversed init() order
        _mesh.destroy();
        _texture.destroy();
        _pipeline_vertcols.destroy();
        _pipeline_textured.destroy();
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
        _time.update();
        // choose color to clear screen with
        glClearColor(0.01, 0.01, 0.01, 0.0);
        // clear image before drawing to it
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // move via WASDQE
        float speed = 2.0 * _time._delta; // 2.0 units per second
        if (Keys::down(SDLK_W)) _camera.translate(0, 0, -speed);
        if (Keys::down(SDLK_A)) _camera.translate(-speed, 0, 0);
        if (Keys::down(SDLK_S)) _camera.translate(0, 0, +speed);
        if (Keys::down(SDLK_D)) _camera.translate(+speed, 0, 0);
        if (Keys::down(SDLK_Q)) _camera.translate(0, -speed, 0);
        if (Keys::down(SDLK_E)) _camera.translate(0, +speed, 0);

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

        // make the transform spin
        _transform._rotation += 0.5 * _time._delta;
        _transform._scale = glm::vec3(1.0, 1.0, 1.0) + glm::vec3(1, 1, 1) * float(std::sin(_time._total) * 0.2);

        /// excuting the pipeline for textured drawing
        // bind graphics pipeline containing vertex and fragment shaders
        _pipeline_textured.bind();
        // bind camera to the pipeline
        _camera.bind();
        // bind and draw mesh
        _transform._position.x = -2;
        _transform.bind();
        _texture.bind();
        _mesh.bind();
        _mesh.draw();

        /// executing the pipeline for vertex color drawing
        _pipeline_vertcols.bind();
        _camera.bind();
        _transform._position.x = +2;
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

    Time _time;
    Mesh _mesh;
    Window _window;
    Camera _camera;
    Texture _texture;
    Pipeline _pipeline_textured;
    Pipeline _pipeline_vertcols;
    Transform _transform;
};