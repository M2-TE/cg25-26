#pragma once
#include "mesh.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "transform.hpp"

struct Model {
    void init(Primitive primitive) {
        _mesh.init(primitive);
        _material._texture_contribution = 0.0f;
    }
    void init(Primitive primitive, const std::string& texture_path) {
        _mesh.init(primitive);
        _texture.init(texture_path);
        _material._texture_contribution = 1.0f;
    }
    void destroy() {
        _mesh.destroy();
        _texture.destroy();
    }
    void draw() {
        // only bind the texture when necessary
        if (_material._texture_contribution > 0.0f) {
            _texture.bind();
        }
        _material.bind();
        _transform.bind();
        _mesh.bind();
        _mesh.draw();
    }

    Mesh _mesh;
    Texture _texture;
    Material _material;
    Transform _transform;
};
