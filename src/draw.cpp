/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/Font.hpp>
#include <glez/detail/program.hpp>
#include "glez/detail/freetype-gl/vertex-buffer.hpp"
#include <cstring>
#include <cmath>
#include <glez/draw.hpp>
#include <glez/Render.hpp>

static ftgl::VertexBuffer<glez::render::vertex> vertex_buffer{ "vertex:2f,tex_coord:2f,color:4f,drawmode:1i" };

struct circle_vertex
{
    ftgl::vec2 position;
    ftgl::vec2 center;
    float radius_inner;
    float radius_outer;
    glez::rgba color;
};

static ftgl::VertexBuffer<circle_vertex> circle_buffer{ "position:2f,center:2f,radius_inner:1f,radius_outer:1f,color:4f" };


static void internal_string(float x, float y, std::string_view string, ftgl::TextureFont& font, glez::rgba color)
{
    glez::render::useProgram(glez::detail::program::shaderIdentity());
    glez::render::bindVertexBuffer(&vertex_buffer, GL_TRIANGLES);
    glez::render::bindTexture(font.atlas);

    glez::render::vertex vertices[string.size() * 4];
    GLuint indices[string.size() * 6];

    for (auto i = 0u, j = 0u; i < string.size() * 6; i += 6, j += 4)
    {
        indices[i + 0] = 0 + j;
        indices[i + 1] = 1 + j;
        indices[i + 2] = 2 + j;
        indices[i + 3] = 2 + j;
        indices[i + 4] = 3 + j;
        indices[i + 5] = 0 + j;
    }
    for (auto& vertex: vertices)
    {
        vertex.color = color;
        vertex.mode = static_cast<int>(glez::detail::program::mode::FREETYPE);
    }

    y += font.height / 1.5f;

    for (auto i = 0u, v = 0u; i < string.size(); ++i, v += 4)
    {
        auto glyph = font.get_glyph(string.data() + i);
        if (glyph == nullptr)
            continue;
        if (i > 0)
            x += glyph->get_kerning(string.data() + i - 1);

        const float x0 = (int) (x + glyph->offset_x);
        const float y0 = (int) (y - glyph->offset_y);
        const float x1 = (int) (x0 + glyph->width);
        const float y1 = (int) (y0 + glyph->height);
        const float s0 = glyph->s0;
        const float t0 = glyph->t0;
        const float s1 = glyph->s1;
        const float t1 = glyph->t1;

        vertices[v + 0].position = { x0, y0 };
        vertices[v + 0].uv       = { s0, t0 };

        vertices[v + 1].position = { x0, y1 };
        vertices[v + 1].uv       = { s0, t1 };

        vertices[v + 2].position = { x1, y1 };
        vertices[v + 2].uv       = { s1, t1 };

        vertices[v + 3].position = { x1, y0 };
        vertices[v + 3].uv       = { s1, t0 };

        x += glyph->advance_x;
    }

    vertex_buffer.push_back(vertices, 4 * string.size(), indices, 6 * string.size());
}

namespace indices
{

static GLuint rectangle[6] = { 0, 1, 2, 2, 3, 0 };
}



namespace glez::draw
{

void rect(float x, float y, float w, float h, rgba color)
{
    render::useProgram(detail::program::shaderIdentity());
    render::bindVertexBuffer(&vertex_buffer, GL_TRIANGLES);

    render::vertex vertices[4];

    for (auto &vertex : vertices)
    {
        vertex.mode  = static_cast<int>(detail::program::mode::PLAIN);
        vertex.color = color;
    }

    vertices[0].position = { x, y };
    vertices[1].position = { x, y + h };
    vertices[2].position = { x + w, y + h };
    vertices[3].position = { x + w, y };

    vertex_buffer.push_back(vertices, 4, indices::rectangle, 6);
}


void
rect(float x, float y, float w, float h, rgba color_nw, rgba color_ne,
           rgba color_se, rgba color_sw)
{
    render::useProgram(detail::program::shaderIdentity());
    render::bindVertexBuffer(&vertex_buffer, GL_TRIANGLES);

    render::vertex vertices[4];

    for (auto& vertex: vertices)
        vertex.mode = static_cast<int>(detail::program::mode::PLAIN);

    vertices[0].position = { x, y };
    vertices[0].color = color_nw;
    vertices[1].position = { x, y + h };
    vertices[1].color = color_sw;
    vertices[2].position = { x + w, y + h };
    vertices[2].color = color_se;
    vertices[3].position = { x + w, y };
    vertices[3].color = color_ne;

    vertex_buffer.push_back(vertices, 4, indices::rectangle, 6);
}

void rect_outline(float x, float y, float w, float h, rgba color, float thickness)
{
    rect(x, y, w, thickness, color);
    rect(x, y, thickness, h, color);
    rect(x + w - thickness, y, thickness, h, color);
    rect(x, y + h - thickness, w, thickness, color);
}

void string(float x, float y, std::string_view string, Font &font, rgba color)
{
    internal_string(x, y, string, *font.font, color);
}

void outlined_string(float x, float y, std::string_view string, Font &font,
                     rgba color, rgba outline)
{
    font.font->outline_thickness = 1.0f;
    font.font->rendermode = ftgl::rendermode_t::RENDER_OUTLINE_POSITIVE;
    internal_string(x, y, string, *font.font, outline);
    font.font->outline_thickness = 0.0f;
    font.font->rendermode = ftgl::rendermode_t::RENDER_NORMAL;
    internal_string(x, y, string, *font.font, color);
}

void line(float x, float y, float dx, float dy, rgba color, float thickness)
{
    line(x, y, dx, dy, color, color, thickness);
}

void line(float x, float y, float dx, float dy, rgba color_begin,
                rgba color_end, float thickness)
{
    render::useProgram(detail::program::shaderIdentity());
    render::bindVertexBuffer(&vertex_buffer, GL_TRIANGLES);

    // Dirty
    x += 0.5f;
    y += 0.5f;

    float length = sqrtf(dx * dx + dy * dy);
    dx *= (length - 1.0f) / length;
    dy *= (length - 1.0f) / length;

    render::vertex vertices[4];

    for (auto &vertex : vertices)
    {
        vertex.mode  = static_cast<int>(detail::program::mode::PLAIN);
    }

    vertices[0].color = color_end;
    vertices[1].color = color_begin;
    vertices[2].color = color_begin;
    vertices[3].color = color_end;

    float nx = dx;
    float ny = dy;

    float ex = x + dx;
    float ey = y + dy;

    if (length == 0)
        return;

    nx /= length;
    ny /= length;

    float th = thickness;

    nx *= th * 0.5f;
    ny *= th * 0.5f;

    float px = ny;
    float py = -nx;

    vertices[2].position = { float(x) - nx + px, float(y) - ny + py };
    vertices[1].position = { float(x) - nx - px, float(y) - ny - py };
    vertices[3].position = { ex + nx + px, ey + ny + py };
    vertices[0].position = { ex + nx - px, ey + ny - py };

    vertex_buffer.push_back(vertices, 4, indices::rectangle, 6);
}

void
circle(float x, float y, float radius_inner, float radius_outer, rgba color)
{
    render::useProgram(detail::program::circleShader());
    render::bindVertexBuffer(&circle_buffer, GL_TRIANGLES);

    circle_vertex vertices[4];
    for (auto& vertex: vertices)
    {
        vertex.center = { x, detail::program::getScreenSize().second - y };
        vertex.radius_inner = radius_inner;
        vertex.radius_outer = radius_outer;
        vertex.color = color;
    }

    vertices[0].position = { x - radius_outer, y - radius_outer };
    vertices[1].position = { x - radius_outer, y + radius_outer };
    vertices[2].position = { x + radius_outer, y + radius_outer };
    vertices[3].position = { x + radius_outer, y - radius_outer };

    circle_buffer.push_back(vertices, 4, indices::rectangle, 6);
}

void
rect_textured(float x, float y, float w, float h, rgba color, Texture &texture,
              float tx, float ty, float tw, float th, float angle)
{
    render::useProgram(detail::program::shaderIdentity());
    render::bindTexture(texture);
    render::bindVertexBuffer(&vertex_buffer, GL_TRIANGLES);

    render::vertex vertices[4];

    for (auto &vertex : vertices)
    {
        vertex.mode  = static_cast<int>(detail::program::mode::TEXTURED);
        vertex.color = color;
    }

    vertices[0].position = { x, y };
    vertices[1].position = { x, y + h };
    vertices[2].position = { x + w, y + h };
    vertices[3].position = { x + w, y };

    if (angle != 0.0f)
    {
        float cx = x + float(w) / 2.0f;
        float cy = y + float(h) / 2.0f;

        for (auto &v : vertices)
        {
            float ox = v.position.x;
            float oy = v.position.y;

            v.position.x =
                    cx + cosf(angle) * (ox - cx) - sinf(angle) * (oy - cy);
            v.position.y =
                    cy + sinf(angle) * (ox - cx) + cosf(angle) * (oy - cy);
        }
    }

    float s0 = float(tx) / texture.getWidth();
    float s1 = float(tx + tw) / texture.getWidth();
    float t0 = float(ty) / texture.getHeight();
    float t1 = float(ty + th) / texture.getHeight();

    vertices[0].uv = { s0, t0 };
    vertices[1].uv = { s0, t1 };
    vertices[2].uv = { s1, t1 };
    vertices[3].uv = { s1, t0 };

    vertex_buffer.push_back(vertices, 4, indices::rectangle, 6);
}

}