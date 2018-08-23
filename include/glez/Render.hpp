/*
  Created on 03.08.18.
*/

#pragma once

#include <GL/glew.h>
#include <glez/color.hpp>
#include <glez/detail/freetype-gl/vertex-buffer.hpp>
#include <glez/Font.hpp>

namespace glez::render
{

struct vertex
{
    ftgl::vec2 position;
    ftgl::vec2 uv;
    glez::rgba color;
    int mode;
};

void begin();
void end();

/**
 * Clear the current vertex buffer
 */
void reset();

/**
 * Render current vertex array
 */
void commit();

/**
 * Prepare OpenGL state for rendering
 */
void glStateSetup();

/**
 * Restore old OpenGL state
 */
void glStateRestore();

/**
 * Bind a GL texture if needed
 * Only calls glBindTexture if texture name stored internally doesn't match the argument
 * @param texture GL texture name
 */
void bindTexture(GLuint texture);

/**
 * Bind a GL shader if needed
 * Works in similar way to bindTexture
 * @param program GL shader name
 */
void useProgram(GLuint program);

void bindVertexBuffer(ftgl::IVertexBuffer *buffer, GLenum mode);

void bindTextureAtlas(ftgl::TextureAtlas& atlas);

}