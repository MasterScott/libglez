/*
  Created on 04.08.18.
*/

#pragma once

#include <cstddef>
#include <GL/glew.h>
#include <glez/detail/freetype-gl/vertex-buffer.hpp>

namespace glez::queue
{

void begin();

void end();

void render();

size_t getBufferIndex();

//

void commit();

void bindTexture(GLuint texture);

void useProgram(GLuint program);

void bindBuffer(ftgl::IVertexBuffer *buffer, GLenum mode);

void pushIndices(size_t start, size_t count);

}