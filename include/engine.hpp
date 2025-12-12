#pragma once
#include "time.hpp"
#include "input.hpp"
#include "model.hpp"
#include "light.hpp"
#include "audio.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "pipeline.hpp"

struct Engine {
    Engine() {
        _time.init();

        // create render components
        _window.init(1280, 720, 4);
        _pipeline.init("default.vert", "default.frag");
        _shadow_pipeline.init("shadow.vert", "shadow.frag");
        _shadow_pipeline.create_shadow_framebuffer();

        // cube with texture
        _cube_textured.init(Primitive::eCube, "grass.png");
        _cube_textured._transform._position.x = -2;
        // cube with vertex colors
        _cube_vertcols.init(Primitive::eCube);
        _cube_vertcols._transform._position.x = +2;
        // sphere
        _sphere.init(Primitive::eSphere);

        // sponza scene
        _sponza.init("sponza/sponza.obj");
        _sponza._transform._scale = glm::vec3{ 0.01f, 0.01f, 0.01f };

        // move the camera to the back a little
        _camera._position = { 3, 3, 0 };

        // create light for lighting and shadows
        _light.init();

        // create an audio stream for default audio device
        SDL_InitSubSystem(SDL_INIT_AUDIO);
        // load audio file
        _doom.init("assets/audio/doom.wav");
        // create an audio stream for current playback device
        audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr, nullptr, nullptr);
        if (audio_stream == nullptr) std::println("{}", SDL_GetError());
        // get the format of the device (sample rate and such)
        SDL_AudioSpec device_format;
        SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &device_format, nullptr);
        // set up the audio stream to convert from our .wav file sample rate to the device's sample rate
        if (!SDL_SetAudioStreamFormat(audio_stream, &_doom.spec, &device_format)) std::println("{}", SDL_GetError());
        // load .wav into the audio stream and play
        if (!SDL_PutAudioStreamData(audio_stream, _doom.buffer, _doom.buffer_size)) std::println("{}", SDL_GetError());
        if (!SDL_ResumeAudioStreamDevice(audio_stream)) std::println("{}", SDL_GetError());
    }
    ~Engine() {
        // destroy in reversed init() order
        SDL_DestroyAudioStream(audio_stream);
        _sphere.destroy();
        _cube_textured.destroy();
        _cube_vertcols.destroy();
        _pipeline.destroy();
        _shadow_pipeline.destroy();
        _window.destroy();
    }

    void handle_inputs() {
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

        // draw wireframe while holding F
        if (Keys::down(SDLK_F)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
        handle_inputs();

        // draw shadows
        if (_light._shadow_dirty) {
            _shadow_pipeline.bind();
            // render into each cubemap face separately
            for (uint32_t face_i = 0; face_i < 6; face_i++) {
                _light.bind_shadow_write(_shadow_pipeline, face_i);
                // draw the stuff
                _sponza.draw(true);
                _cube_textured.draw(true);
                _cube_vertcols.draw(true);
                _sphere.draw(true);
            }
            _light._shadow_dirty = false;
        }

        // draw color
        {
            // bind graphics pipeline containing vertex and fragment shaders
            _pipeline.bind();
            glViewport(0, 0, 1280, 720); // TODO: dynamically set to size of window
            // choose color to clear screen with
            glClearColor(0.01, 0.01, 0.01, 0.0);
            // clear image before drawing to it
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            _light.bind_shadow_read();

            // bind camera to the pipeline
            _camera.bind();

            // bind and draw mesh
            _sponza.draw();
            _cube_textured.draw();
            _cube_vertcols.draw();
            _sphere.draw();
        }

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
    Model _sponza;
    Window _window;
    Camera _camera;
    Pipeline _pipeline;
    Pipeline _shadow_pipeline;
    Light _light;
    // audio
    SDL_AudioStream* audio_stream;
    Audio _doom;
};
