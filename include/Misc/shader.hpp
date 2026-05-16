#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>


class Shader{
    public:
        unsigned int vertex,fragment,program;
        Shader(std::string vertexsource,std::string fragmentsource);
        std::string openfile(std::string name);
        ~Shader(){
            glDeleteProgram(program);
        }
};


#endif
