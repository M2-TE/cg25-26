#pragma once
#include "time.hpp"
#include "input.hpp"
#include "model.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "pipeline.hpp"

struct Engine {
    Engine() {
        _time.init();

        // create render components
        _window.init(1280, 720);
        _pipeline.init("default.vert", "default.frag");

        // cube with texture
        _cube_textured.init(Primitive::eCube, "grass.png");
        _cube_textured._transform._position.x = -2;
        // cube with vertex colors
        _cube_vertcols.init(Primitive::eCube);
        _cube_vertcols._transform._position.x = +2;
        // sphere
        _sphere.init(Primitive::eSphere);

        // move the camera to the back a little
        _camera._position.z = 5;
    }
    ~Engine() {
        // destroy in reversed init() order
        _sphere.destroy();
        _cube_textured.destroy();
        _cube_vertcols.destroy();
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

        // bind graphics pipeline containing vertex and fragment shaders
        _pipeline.bind();
        // bind camera to the pipeline
        _camera.bind();
        // bind and draw mesh
        _cube_textured.draw();
        _cube_vertcols.draw();
        _sphere.draw();

        // present drawn image to screen
        SDL_GL_SwapWindow(_window._window_p);
        // clear single-frame inputs
        Input::flush();
        return SDL_AppResult::SDL_APP_CONTINUE;
    }

    Time _time;
    Model _cube_vertcols;
    Model _cube_textured;
    Model _sphere;
    Window _window;
    Camera _camera;
    Pipeline _pipeline;
};
