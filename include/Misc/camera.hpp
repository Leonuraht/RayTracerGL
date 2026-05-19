#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Frustum{
    float line_right_m,line_right_c;
    float line_left_m,line_left_c;
};


class Camera {
  public:
    bool isDirty = true,firstmouse = true;
    float FOV, aspectratio,near,far,xlast,ylast,mousesens = 0.2f,yaw = -90.0f,pitch=0.0f;
    glm::vec3 pos, dir, target, up, right;
    glm::mat4 proj,view;
    Camera(float FOV, float aspectratio, float near, float far, glm ::vec3 dir,
           glm::vec3 pos);
    void updatevec();
    void updatemat(int projloc, int viewloc);
    void createFrustum();
    bool checkfrustum(int x, int z);
};


#endif
