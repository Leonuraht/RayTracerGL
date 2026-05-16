#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>

const int width = 1066, height = 600;

void framebuffer_cb(GLFWwindow *window, int w, int h);
void process_inp(GLFWwindow *window, double delta);
void glfw_error_callback(int error, const char *description);

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(width, height, "ray_tracer", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, framebuffer_cb);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}
void glfw_error_callback(int error, const char *description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}
