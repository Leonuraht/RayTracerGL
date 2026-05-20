#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#define GLFW_INCLUDE_NONE
#include "Misc/camera.hpp"
#include "Misc/shader.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

struct alignas(16) Object {
  float radius;
  alignas(16) glm::vec3 center;
  alignas(16) glm::vec3 color;
};

const int width = 1066, height = 600;
Camera camera(45.f, (float)width / height, 0.1f, 100.f,
              glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 3.f));

void framebuffer_cb(GLFWwindow *window, int w, int h);
void process_inp(GLFWwindow *window, double delta);
void glfw_error_callback(int error, const char *description);
void mouse_cb(GLFWwindow *window, double x, double y);
std::vector<Object> object{
    Object(0.5f, glm::vec3(-0.2f, 0.2f, -1.f), glm::vec3(1.f, 0.4f, 0.5f)),
    Object(0.2f, glm::vec3(0.f, -0.5f, 2.f), glm::vec3(0.5f, 0.2f, 0.3f)),
    Object(0.8f, glm::vec3(-1.f, 0.f, -4.f), glm::vec3(0.6f, 0.7f, 0.4f)),
    Object(-8.f, glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.5f, 0.5f, 0.1f))};

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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_cb);

  GLuint ssbo;
  glGenBuffers(1, &ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
  glBufferStorage(GL_SHADER_STORAGE_BUFFER, object.size() * sizeof(Object),
                  object.data(), 0);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);

  const unsigned int tw = 1024 / 1, th = 1024 / 1;
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
  GLuint delt = glGetUniformLocation(shader_compute.program, "delta"),
         cam_dir = glGetUniformLocation(shader_compute.program, "camera.dir"),
         light_dir =
             glGetUniformLocation(shader_compute.program, "dirlight.dir"),
         cam_pos = glGetUniformLocation(shader_compute.program, "camera.pos"),
         cam_up = glGetUniformLocation(shader_compute.program, "camera.up"),
         cam_right =
             glGetUniformLocation(shader_compute.program, "camera.right"),
         cam_fov = glGetUniformLocation(shader_compute.program, "camera.FOV"),
         lightcol =
             glad_glGetUniformLocation(shader_compute.program, "dirlight.col");
  glUniform1f(cam_fov, camera.FOV / 2.f);
  glUniform3f(light_dir, -0.3f, -0.2f, -1.f);
  glUniform3f(lightcol, 0.2f, 0.3f, 0.5f);

  glUseProgram(shader_std.program);
  GLuint text0 = glGetUniformLocation(shader_std.program, "text0");
  glUniform1i(text0, 0);

  glBindVertexArray(VAO);
  double pastime = glfwGetTime(), time = 0, delta = 0;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    time = glfwGetTime();
    delta = time - pastime;
    pastime = time;
    camera.updatevec();

    glUseProgram(shader_compute.program);
    glUniform1f(delt, time);
    glUniform3f(cam_dir, camera.dir.x, camera.dir.y, camera.dir.z);
    glUniform3f(cam_pos, camera.pos.x, camera.pos.y, camera.pos.z);
    glUniform3f(cam_up, camera.up.x, camera.up.y, camera.up.z);
    glUniform3f(cam_right, camera.right.x, camera.right.y, camera.right.z);

    glDispatchCompute(32, 32, 1);
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

void glfw_error_callback(int error, const char *description) {
  std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void process_inp(GLFWwindow *window, double delta) {
  float camspd = 5 * delta;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, 1);
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.isDirty = true;
    camera.pos += camera.dir * camspd;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.isDirty = true;
    camera.pos -= camera.right * camspd;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.isDirty = true;
    camera.pos -= camera.dir * camspd;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.isDirty = true;
    camera.pos += camera.right * camspd;
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
  camera.yaw -= xoff;

  if (camera.pitch > 89.0f)
    camera.pitch = 89.0f;
  if (camera.pitch < -89.0f)
    camera.pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  direction.y = sin(glm::radians(camera.pitch));
  direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
  camera.dir = glm::normalize(direction);
}