#include "Misc/camera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float FOV, float aspectratio, float near, float far,
               glm ::vec3 dir, glm::vec3 pos) {
    this->FOV = FOV;
    this->aspectratio = aspectratio;
    this->dir = dir;
    this->pos = pos;
    this->far = far;
    this->near = near;
    this->right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir));
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
void Camera::updatemat(int projloc, int viewloc) {
    if (!isDirty)
        return;
    proj = glm::perspective(FOV, aspectratio, near, far);
    view = glm::lookAt(pos, pos + dir, up);

    glUniformMatrix4fv(projloc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
    isDirty = false;
}

bool Camera::checkfrustum(int x, int z) {
    glm::vec2 campos(pos.x, pos.z);
    float xpos = x * 16, zpos = z * 16;
    float xposmax = xpos + 16, zposmax = zpos + 16;
    glm::vec2 vert[4] = {glm::vec2(xpos, zpos), glm::vec2(xpos, zposmax),
                         glm::vec2(xposmax, zpos), glm::vec2(xposmax, zposmax)

    };
    glm::vec2 lookdir = glm::vec2(dir.x, dir.z);
    for (int i = 0; i < 4; i++) {
        glm::vec2 camdir = vert[i] - campos;
        if (glm::dot(camdir, camdir) < 255.0f)
            return 1;
        if (glm::dot(camdir, lookdir) >= 0.0)
            return 1;
    }

    return 0;
}
