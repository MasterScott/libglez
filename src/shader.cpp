/*
  Created on 31.07.18.
*/

#include <GL/glew.h>
#include <stdio.h>
#include <stdexcept>
#include <glez/shader.hpp>
#include <cassert>

GLuint glez::shader::compile(const char *source, GLenum type)
{
    GLint status;
    GLuint result = glCreateShader(type);

    glShaderSource(result, 1, &source, nullptr);
    glCompileShader(result);
    glGetShaderiv(result, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE)
    {
        char error[512];
        GLsizei length;
        glGetShaderInfoLog(result, 512, &length, error);
        fprintf(stderr, "GLEZ: Shader compile error: %s, %s\n", error, source);
        throw std::runtime_error("Shader compile error: " + std::string(error));
    }

    return result;
}

GLuint glez::shader::link(GLuint vertex, GLuint fragment)
{
    GLuint result = glCreateProgram();
    GLint status;

    glAttachShader(result, vertex);
    glAttachShader(result, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glLinkProgram(result);
    glGetProgramiv(result, GL_LINK_STATUS, &status);

    assert(status == GL_TRUE);

    return result;
}
