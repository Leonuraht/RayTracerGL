#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Misc/camera.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

const int width = 1066, height = 600;

void framebuffer_cb(GLFWwindow *window, int w, int h);
void mouse_cb(GLFWwindow *window, double x, double y);
void scroll_cb(GLFWwindow *window, double x, double y);
void process_inp(GLFWwindow *window, double delta);

Camera camera(45.f, (float)width / height, 0.1f, 1000.f,
              glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 3.0));

int main() {
    glfwInit();
    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(width, height, "Ray Tracer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "WINDOW CREATION FAILED" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GALD LOADING FAILED" << std::endl;
        return -1;
    }
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_cb);

    double pastime = glfwGetTime(), time = 0, delta = 0;
    while (!glfwWindowShouldClose(window)) {
        time = glfwGetTime();
        delta = time - pastime;
        pastime = time;
        process_inp(window, delta);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_cb(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
}

void process_inp(GLFWwindow *window, double delta) {
    float camspd = 20 * delta;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos += camera.dir * camspd;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos += camera.right * camspd;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos -= camera.dir * camspd;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos -= camera.right * camspd;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos += glm::vec3(0.0f, camspd, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        camera.isDirty = true;
        camera.pos -= glm::vec3(0.0f, camspd, 0.0f);
    }
}
void mouse_cb(GLFWwindow *window, double x, double y) {
    camera.isDirty = true;
    if (camera.firstmouse) {
        camera.xlast = x;
        camera.ylast = y;
        camera.firstmouse = false;
    }
    float xoff = (x - camera.xlast) * camera.mousesens;
    float yoff = (y - camera.ylast) * camera.mousesens;
    camera.xlast = x;
    camera.ylast = y;

    camera.pitch -= yoff;
    camera.yaw += xoff;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    glm::vec3 direction;
    direction.x =
        cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    direction.y = sin(glm::radians(camera.pitch));
    direction.z =
        sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.dir = glm::normalize(direction);
}

void scroll_cb(GLFWwindow *window, double x, double y) {
    camera.isDirty = true;
    camera.FOV -= (float)y * 0.05;
    if (camera.FOV > 90.0f)
        camera.FOV = 90.0f;
    if (camera.FOV < 1.0f)
        camera.FOV = 1.0f;
}
