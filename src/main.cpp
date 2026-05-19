#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include "Misc/camera.hpp"
#include "Misc/shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

const int width = 1066, height = 600;

void framebuffer_cb(GLFWwindow *window, int w, int h);
void process_inp(GLFWwindow *window, double delta);
void glfw_error_callback(int error, const char *description);
Camera camera(45.f,(float) width/height,0.1f,100.f,glm::vec3(0.f,0.f,-1.f),glm::vec3(0.f,0.f,3.f));

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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const unsigned int tw = 1024, th = 1024;
    unsigned int texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tw, th, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    glBindImageTexture(0, texture0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    Shader shader_compute(
        "/home/leonuraht/storage/CFiles/RayTracer/src/shaders/compute.glsl");
    Shader shader_std(
        "/home/leonuraht/storage/CFiles/RayTracer/src/shaders/vertex.glsl",
        "/home/leonuraht/storage/CFiles/RayTracer/src/shaders/fragment.glsl");
    
    glUseProgram(shader_compute.program);
    GLuint delt = glGetUniformLocation(shader_compute.program, "delta");
     
    glBindVertexArray(VAO);
    glUseProgram(shader_std.program);
    GLuint text0 = glGetUniformLocation(shader_std.program, "text0");
    glUniform1i(text0, 0);

    double pastime = glfwGetTime(), time = 0, delta = 0;
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        time = glfwGetTime();
        delta = time - pastime;
        pastime = time;
        glUseProgram(shader_compute.program);
        glUniform1f(delt, time);
        glDispatchCompute(64, 64, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        process_inp(window, delta);


        glUseProgram(shader_std.program);
        glDrawArrays(GL_TRIANGLES, 0, 6);
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
