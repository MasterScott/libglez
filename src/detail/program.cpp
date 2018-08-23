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
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
            gl_FragColor = vec4(frag_Color.rgb, frag_Color.a * tex.r);
       }
       else
           gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
)END";

static const char *shader_circle_vertex = R"END(
#version 130
in vec2 position;
in vec2 center;
in float radius_inner;
in float radius_outer;
in vec4 color;
uniform mat4 projection;

out float frag_RadiusInner;
out float frag_RadiusOuter;
out vec4 frag_Color;
out vec2 frag_Center;

void main()
{
    gl_Position   = projection*vec4(position,0.0,1.0);
    frag_RadiusInner = radius_inner;
    frag_RadiusOuter = radius_outer;
    frag_Color = color;
    frag_Center = center;
}
)END";

static const char *shader_circle_fragment = R"END(
#version 130
in float frag_RadiusInner;
in float frag_RadiusOuter;
in vec4 frag_Color;
in vec2 frag_Center;

void main()
{
    float dist = distance(gl_FragCoord.xy, frag_Center);
    if (dist > frag_RadiusOuter)
        discard;
    if (dist < frag_RadiusInner)
        discard;
    gl_FragColor = frag_Color;
}
)END";

static GLuint shader_identity{};
static GLuint shader_circle{};

static int screen_width{ 0 };
static int screen_height{ 0 };

namespace glez::detail::program
{

void resize(int width, int height)
{
    screen_width = width;
    screen_height = height;

    ftgl::mat4 projection{};
    projection.set_identity();
    projection.set_orthographic(0, width, height, 0, -1, 1);

    glUseProgram(shader_identity);
    glUniformMatrix4fv(glGetUniformLocation(shader_identity, "projection"), 1, 0,
                       projection.data);
    glUseProgram(0);

    glUseProgram(shader_circle);
    glUniformMatrix4fv(glGetUniformLocation(shader_circle, "projection"), 1, 0,
                       projection.data);
    glUseProgram(0);
}

void init(int width, int height)
{
    shader_identity = shader::link(shader::compile(shader_vertex, GL_VERTEX_SHADER),
                  shader::compile(shader_fragment, GL_FRAGMENT_SHADER));

    shader_circle = shader::link(shader::compile(shader_circle_vertex, GL_VERTEX_SHADER),
            shader::compile(shader_circle_fragment, GL_FRAGMENT_SHADER));

    resize(width, height);

    glUseProgram(shader_identity);
    glUniform1i(glGetUniformLocation(shader_identity, "texture"), 0);
    glUseProgram(0);
}

void shutdown()
{
    glDeleteProgram(shader_identity);
    glDeleteProgram(shader_circle);
}

void end()
{
    glUseProgram(0);
}

GLuint shaderIdentity()
{
    return shader_identity;
}

GLuint circleShader()
{
    return shader_circle;
}

std::pair<int, int> getScreenSize()
{
    return std::make_pair(screen_width, screen_height);
}

}