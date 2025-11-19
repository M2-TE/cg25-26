#pragma once

struct Texture {
    void init(const std::string& path) {
        // base directory of the executable
        std::string base_path = SDL_GetBasePath();
        std::string full_path = base_path + "assets/textures/" + path;
        
        // load image from disk
        SDL_Surface* texture_raw_p = IMG_Load(full_path.c_str());
        if (texture_raw_p == nullptr) {
            std::println("IMG_Load(\"{}\"): {}", path, SDL_GetError());
            std::exit(1);
        }

        // store width and height
        _width = texture_raw_p->w;
        _height = texture_raw_p->h;

        // create texture to store image in (texture is gpu buffer)
        glCreateTextures(GL_TEXTURE_2D, 1, &_texture);

        // upload to the GPU
        glTextureStorage2D(_texture, 1, GL_SRGB8, _width, _height);
        glTextureSubImage2D(_texture, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, texture_raw_p->pixels);

        // free image on cpu side
        SDL_DestroySurface(texture_raw_p);

        // set sampler parameters
        glTextureParameteri(_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // interpolation mode when scaling image down
        glTextureParameteri(_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // interpolation mode when scaling image up
    }
    void destroy() {
        glDeleteTextures(1, &_texture);
    }
    void bind() {
        glBindTextureUnit(0, _texture);
    }

    GLuint _texture;
    GLuint _width;
    GLuint _height;
};