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

        ////// create shadow stuff
        // create shadow texture as cube map
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_shadow_texture);
        glTextureStorage2D(_shadow_texture, 1, GL_DEPTH_COMPONENT32F, _shadow_width, _shadow_height); // 32F (32-bit float) per pixel
        // create shadow camera matrices
        _shadow_projection = glm::perspectiveFov<float>(glm::radians(90.0f), _shadow_width, _shadow_height, 1.0f, _light_range);
        _shadow_views[0] = glm::lookAt(_light_position, _light_position + glm::vec3(+1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // right
        _shadow_views[1] = glm::lookAt(_light_position, _light_position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // left
        _shadow_views[2] = glm::lookAt(_light_position, _light_position + glm::vec3( 0.0f, +1.0f,  0.0f), glm::vec3(0.0f,  0.0f, +1.0f)); // top
        _shadow_views[3] = glm::lookAt(_light_position, _light_position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)); // bottom
        _shadow_views[4] = glm::lookAt(_light_position, _light_position + glm::vec3( 0.0f,  0.0f, +1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // back
        _shadow_views[5] = glm::lookAt(_light_position, _light_position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)); // front
    }
    ~Engine() {
        // destroy in reversed init() order
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
        if (_shadow_dirty) {
            _shadow_pipeline.bind();
            glViewport(0, 0, _shadow_width, _shadow_height);
            // render into each cubemap face separately
            for (uint32_t face_i = 0; face_i < 6; face_i++) {
                // bind the target shadow map
                glNamedFramebufferTextureLayer(_shadow_pipeline._framebuffer, GL_DEPTH_ATTACHMENT, _shadow_texture, 0, face_i);
                // clear it before doing any writing
                glClear(GL_DEPTH_BUFFER_BIT);
                // bind the light view+projection matrices (act like it is the camera)
                glUniformMatrix4fv( 8, 1, false, glm::value_ptr(_shadow_views[face_i]));
                glUniformMatrix4fv(12, 1, false, glm::value_ptr(_shadow_projection));
                // draw the stuff
                _sponza.draw(true);
                _cube_textured.draw(true);
                _cube_vertcols.draw(true);
                _sphere.draw(true);
            }
            _shadow_dirty = false;
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

            // bind shadow textures for reading (slot 1)
            glBindTextureUnit(1, _shadow_texture);

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

    // shadow settings
    bool _shadow_dirty = true; // only render shadows when lights or objects moved!
    float _light_range = 100.0f; // TODO: upload to fragment shader
    glm::vec3 _light_position{ 3, 3, 0 }; // TODO: upload to FS
    glm::vec3 _light_color{ 0.992, 0.984, 0.827 }; // TODO: upload to FS
    GLuint _shadow_width = 4096;
    GLuint _shadow_height = 4096;
    GLuint _shadow_texture; // cubemap (6 textures)
    glm::mat4x4 _shadow_projection;
    std::array<glm::mat4x4, 6> _shadow_views;
};
