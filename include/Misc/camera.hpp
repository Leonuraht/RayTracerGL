#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
  public:
    bool isDirty = true,firstmouse = true;
    float FOV, aspectratio,near,far,xlast,ylast,mousesens = 0.07f,yaw = -90.0f,pitch=0.0f;
    glm::vec3 pos, dir, target, up, right;
    Camera(float FOV, float aspectratio, float near, float far, glm ::vec3 dir,
           glm::vec3 pos);
    void updatevec();
};


#endif
