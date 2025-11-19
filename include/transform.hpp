#pragma once

struct Transform {
    void bind() {
        // initialize to identity matrix
        glm::mat4x4 transform_matrix(1.0);
        // calculate transform/model matrix from transform components
        transform_matrix = glm::translate(transform_matrix, _position);
        transform_matrix = glm::rotate(transform_matrix, _rotation.x, glm::vec3(1, 0, 0));
        transform_matrix = glm::rotate(transform_matrix, _rotation.y, glm::vec3(0, 1, 0));
        transform_matrix = glm::rotate(transform_matrix, _rotation.z, glm::vec3(0, 0, 1));
        transform_matrix = glm::scale(transform_matrix, _scale);
        // upload to GPU
        glUniformMatrix4fv(0, 1, false, glm::value_ptr(transform_matrix));
    }

    glm::vec3 _position{ 0, 0, 0 };
    glm::vec3 _rotation{ 0, 0, 0 }; // euler angles
    glm::vec3 _scale{ 1, 1, 1 };
};