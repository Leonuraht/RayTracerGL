#include "Misc/shader.hpp"
#include <cstdio>
#include <fstream>
#include <sstream>

std::string Shader::openfile(std::string filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void checkshader(unsigned int i,int type) {
    GLint succ;
    glGetShaderiv(i, GL_COMPILE_STATUS, &succ);
    if (!succ) {
        char logdata[512];
        glGetShaderInfoLog(i, 512, NULL, logdata);
        if(type == 0) printf("VERTEX SHADER COMPILATION FAILED : %s\n", logdata);
        else if(type == 1) printf("FRAGMENT SHADER COMPILATION FAILED : %s\n", logdata);
    }
}

void checkprogram(unsigned int i) {
    GLint succ;
    glGetProgramiv(i, GL_LINK_STATUS, &succ);
    if (!succ) {
        char logdata[512];
        glGetProgramInfoLog(i, 512, NULL, logdata);
        printf("PROGRAM COMPILATION FAILED : %s\n", logdata);
    }
}

Shader::Shader(std::string vertexpath, std::string fragmentpath) {
    std::string vert = openfile(vertexpath), frag = openfile(fragmentpath);
    const char *vertex_source = vert.c_str();
    const char *fragment_source = frag.c_str();
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(vertex);
    checkshader(vertex,0);
    glCompileShader(fragment);
    checkshader(fragment,1);
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkprogram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
