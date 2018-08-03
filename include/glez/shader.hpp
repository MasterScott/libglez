/*
  Created on 31.07.18.
*/

#pragma once

#include <GL/gl.h>

namespace glez::shader
{

GLuint compile(const char *source, GLenum type);

GLuint link(GLuint vertex, GLuint fragment);

}