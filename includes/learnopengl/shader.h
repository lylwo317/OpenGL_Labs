//
// Created by kevin on 2022/2/8.
//

#ifndef OPENGL_LABS_SHADER_H
#define OPENGL_LABS_SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


class Shader{
public:
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath){
        std::string vertexCode;
        std::string fragmentCode;

        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        vShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        fShaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

        try{
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            fShaderFile.close();
            vShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ios_base::failure e) {
            std::cout <<  "error shader file not successfully read" << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkShaderCompileErrorInfo(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkShaderCompileErrorInfo(fragment, "FRAGMENT");

        id = glCreateProgram();
        glAttachShader(id, vertex);
        glAttachShader(id, fragment);
        glLinkProgram(id);
        checkProgramCompileErrorInfo(id);

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use(){
        glUseProgram(id);
    };

private:
    void checkShaderCompileErrorInfo(unsigned int shader, const std::string& type){
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cout << "error shader compile error of type: " << type << "\n" << infoLog << std::endl;
        }
    }

    void checkProgramCompileErrorInfo(unsigned int program){
        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (success == GL_FALSE) {
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            std::cout << "error program link error." << "\n" << infoLog << std::endl;
        }
    }
};

#endif //OPENGL_LABS_SHADER_H
