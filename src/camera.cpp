#include "Misc/camera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(float FOV, float aspectratio, float near, float far,
               glm ::vec3 dir, glm::vec3 pos) {
    this->FOV = glm::radians(FOV);
    this->aspectratio = aspectratio;
    this->dir = dir;
    this->pos = pos;
    this->far = far;
    this->near = near;
    this->right = glm::normalize(glm::cross(dir,glm::vec3(0.0f, 1.0f, 0.0f)));
    this->up = glm::cross(dir, right);
}
void Camera::updatevec() {
    if (!isDirty)
        return;
    if (dir != glm::vec3(0.0f, 1.0f, 0.0f))
        this->right =
            glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir));
    this->up = glm::cross(dir, right);
}