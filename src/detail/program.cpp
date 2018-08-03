/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/detail/freetype-gl/freetype-gl.hpp>
#include "glez/detail/freetype-gl/vertex-buffer.hpp"
#include "glez/detail/freetype-gl/mat4.hpp"
#include <glez/detail/program.hpp>
#include <cstdio>
#include <stdexcept>
#include <cassert>
#include <glez/shader.hpp>

static const char *shader_vertex = R"END(
#version 130
uniform mat4 projection;
in vec2 vertex;
in vec2 tex_coord;
in vec4 color;
in int drawmode;
flat out int frag_DrawMode;
out vec4 frag_Color;
out vec2 frag_TexCoord;
void main()
{
    gl_Position   = projection*vec4(vertex + vec2(0.0, -0.05),0.0,1.0);
    frag_TexCoord = tex_coord;
    frag_Color = color;
    frag_DrawMode = drawmode;
}
)END";

static const char *shader_fragment = R"END(
#version 130
uniform sampler2D texture;
in vec4 frag_Color;
in vec2 frag_TexCoord;
flat in int frag_DrawMode;
void main()
{
   if (frag_DrawMode == 1)
       gl_FragColor = frag_Color;
   else
   {
       vec4 tex = texture2D(texture, frag_TexCoord);
       if (frag_DrawMode == 2)
           gl_FragColor = frag_Color * tex;
       else if (frag_DrawMode == 3)
       {
           gl_FragColor = vec4(frag_Color.rgb, frag_Color.a * tex.r);
       }
       else
           gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
)END";

static GLuint shader_identity{};

namespace glez::detail::program
{

ftgl::VertexBuffer<glez::detail::render::vertex> buffer{ "vertex:2f,tex_coord:2f,color:4f,drawmode:1i" };

void resize(int width, int height)
{
    glUseProgram(shader_identity);
    ftgl::mat4 projection{};
    projection.set_identity();
    projection.set_orthographic(0, width, height, 0, -1, 1);
    glUniformMatrix4fv(glGetUniformLocation(shader_identity, "projection"), 1, 0,
                       projection.data);
    glUseProgram(0);
}

void init(int width, int height)
{
    shader_identity = shader::link(shader::compile(shader_vertex, GL_VERTEX_SHADER),
                  shader::compile(shader_fragment, GL_FRAGMENT_SHADER));

    resize(width, height);

    glUseProgram(shader_identity);
    glUniform1i(glGetUniformLocation(shader_identity, "texture"), 0);
    glUseProgram(0);
}

void shutdown()
{
    glDeleteProgram(shader_identity);
}

void draw()
{
    buffer.render(GL_TRIANGLES);
}

void reset()
{
    buffer.clear();
}

unsigned next_index()
{
    return buffer.vertices.size();
}

void begin()
{
    glUseProgram(shader_identity);
}

void end()
{
    glUseProgram(0);
}

GLuint shaderIdentity()
{
    return shader_identity;
}

}