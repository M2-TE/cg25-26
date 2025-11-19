#pragma once

struct Window { 
    void init() {
        // init the SDL video subsystem before anything else
        bool result_sdl = SDL_InitSubSystem(SDL_INIT_VIDEO);
        if (!result_sdl) std::println("SDL_InitSubSystem: {}", SDL_GetError());

        // OpenGL context settings
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        // create a window specifically with OpenGL support
        _window_p = SDL_CreateWindow("OpenGL Renderer", 1280, 720, SDL_WINDOW_OPENGL);
        if (_window_p == nullptr) std::println("SDL_CreateWindow: {}", SDL_GetError());

        // create opengl context
        _context = SDL_GL_CreateContext(_window_p);
        if (_context == nullptr) std::println("SDL_GL_CreateContext: {}", SDL_GetError());

        // OpenGL function loader
        glbinding::initialize(SDL_GL_GetProcAddress, false);
        glbinding::aux::enableGetErrorCallback();
    }
    void destroy() {
        SDL_Quit();
    }

    SDL_Window* _window_p;
    SDL_GLContext _context;
};