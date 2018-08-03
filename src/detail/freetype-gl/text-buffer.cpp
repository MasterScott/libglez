/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "opengl.hpp"
#include "glez/detail/freetype-gl/text-buffer.hpp"
#include "glez/detail/freetype-gl/utf8-utils.hpp"

namespace ftgl
{

#define SET_GLYPH_VERTEX(value, x0, y0, s0, t0, color, sh, gm) \
    value = { x0, y0, s0, t0, color, sh, gm }

void TextBuffer::text_printf(vec2 *pen, ...)
{
    if (buffer.items.empty())
    {
        origin = *pen;
    }

    va_list args;
    va_start(args, pen);
    do
    {
        auto markup = va_arg(args, const Markup *);
        if (markup == nullptr)
        {
            va_end(args);
            return;
        }
        auto text = va_arg(args, const char *);
        add_text(*pen, *markup, text, 0);
    } while (true);
}

void TextBuffer::add_text(vec2 &pen, const Markup &markup, const char *text,
                          size_t length)
{
    if (!markup.font)
    {
        fprintf(stderr, "Houston, we've got a problem !\n");
        return;
    }

    if (length == 0)
    {
        length = utf8_strlen(text);
    }
    if (buffer.items.empty() == 0)
    {
        origin = pen;
        line_left = pen.x;
        bounds.left = pen.x;
        bounds.top = pen.y;
    }
    else
    {
        if (pen.x < origin.x)
        {
            origin.x = pen.x;
        }
        if (pen.y != last_pen_y)
        {
            finish_line(pen, false);
        }
    }

    const char *prev_character = nullptr;
    for (auto i = 0; length; i += utf8_surrogate_len(text + i))
    {
        add_char(pen, markup, text + i, prev_character);
        prev_character = text + i;
        length--;
    }

    last_pen_y = pen.y;
}

void TextBuffer::add_char(vec2 &pen, const Markup &markup, const char *current,
                          const char *previous)
{
    size_t vcount = 0;
    size_t icount = 0;
    const float gamma = markup.gamma;

    // Maximum number of vertices is 20 (= 5x2 triangles) per glyph:
    //  - 2 triangles for background
    //  - 2 triangles for overline
    //  - 2 triangles for underline
    //  - 2 triangles for strikethrough
    //  - 2 triangles for glyph
    GlyphVertex vertices[4 * 5];
    GLuint indices[6 * 5];
    float kerning = 0.0f;

    if (markup.font->ascender > line_ascender)
    {
        float y = pen.y;
        pen.y -= (markup.font->ascender - line_ascender);
        move_last_line((float)(y - pen.y));
        line_ascender = markup.font->ascender;
    }
    if (markup.font->descender < line_descender)
    {
        line_descender = markup.font->descender;
    }

    if (*current == '\n')
    {
        finish_line(pen, true);
        return;
    }

    auto font = markup.font;
    auto glyph = font->get_glyph(current);
    auto black = font->get_glyph(nullptr);

    if (glyph == nullptr)
        return;

    if (previous && markup.font->kerning)
        kerning = glyph->get_kerning(previous);

    pen.x += kerning;

    // Background
    if (markup.background_color.alpha > 0.0f)
    {
        float x0 = (pen.x - kerning);
        float y0 = (float) (int) (pen.y + font->descender);
        float x1 = (x0 + glyph->advance_x);
        float y1 = (float) (int) (y0 + font->height + font->linegap);
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;

        SET_GLYPH_VERTEX(vertices[vcount + 0], (float) (int) x0, y0, s0, t0,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 1], (float) (int) x0, y1, s0, t1,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 2], (float) (int) x1, y1, s1, t1,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 3], (float) (int) x1, y0, s1, t0,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        indices[icount + 0] = vcount + 0;
        indices[icount + 1] = vcount + 1;
        indices[icount + 2] = vcount + 2;
        indices[icount + 3] = vcount + 0;
        indices[icount + 4] = vcount + 2;
        indices[icount + 5] = vcount + 3;
        vcount += 4;
        icount += 6;
    }

    // Underline
    if (markup.underline)
    {
        float x0 = (pen.x - kerning);
        float y0 = (float) (int) (pen.y + font->underline_position);
        float x1 = (x0 + glyph->advance_x);
        float y1 = (float) (int) (y0 + font->underline_thickness);
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;

        SET_GLYPH_VERTEX(vertices[vcount + 0], (float) (int) x0, y0, s0, t0,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 1], (float) (int) x0, y1, s0, t1,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 2], (float) (int) x1, y1, s1, t1,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 3], (float) (int) x1, y0, s1, t0,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        indices[icount + 0] = vcount + 0;
        indices[icount + 1] = vcount + 1;
        indices[icount + 2] = vcount + 2;
        indices[icount + 3] = vcount + 0;
        indices[icount + 4] = vcount + 2;
        indices[icount + 5] = vcount + 3;
        vcount += 4;
        icount += 6;
    }

    // Overline
    if (markup.overline)
    {
        float x0 = (pen.x - kerning);
        float y0 = (float) (int) (pen.y + (int) font->ascender);
        float x1 = (x0 + glyph->advance_x);
        float y1 = (float) (int) (y0 + (int) font->underline_thickness);
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;
        SET_GLYPH_VERTEX(vertices[vcount + 0], (float) (int) x0, y0, s0, t0,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 1], (float) (int) x0, y1, s0, t1,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 2], (float) (int) x1, y1, s1, t1,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 3], (float) (int) x1, y0, s1, t0,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        indices[icount + 0] = vcount + 0;
        indices[icount + 1] = vcount + 1;
        indices[icount + 2] = vcount + 2;
        indices[icount + 3] = vcount + 0;
        indices[icount + 4] = vcount + 2;
        indices[icount + 5] = vcount + 3;
        vcount += 4;
        icount += 6;
    }

    /* Strikethrough */
    if (markup.strikethrough)
    {
        float x0 = (pen.x - kerning);
        float y0 = (float) (int) (pen.y + (int) font->ascender * .33f);
        float x1 = (x0 + glyph->advance_x);
        float y1 = (float) (int) (y0 + (int) font->underline_thickness);
        float s0 = black->s0;
        float t0 = black->t0;
        float s1 = black->s1;
        float t1 = black->t1;
        SET_GLYPH_VERTEX(vertices[vcount + 0], (float) (int) x0, y0, s0, t0,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 1], (float) (int) x0, y1, s0, t1,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 2], (float) (int) x1, y1, s1, t1,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 3], (float) (int) x1, y0, s1, t0,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        indices[icount + 0] = vcount + 0;
        indices[icount + 1] = vcount + 1;
        indices[icount + 2] = vcount + 2;
        indices[icount + 3] = vcount + 0;
        indices[icount + 4] = vcount + 2;
        indices[icount + 5] = vcount + 3;
        vcount += 4;
        icount += 6;
    }
    {
        // Actual glyph
        float x0 = (pen.x + glyph->offset_x);
        float y0 = (float) (int) (pen.y + glyph->offset_y);
        float x1 = (x0 + glyph->width);
        float y1 = (float) (int) (y0 - glyph->height);
        float s0 = glyph->s0;
        float t0 = glyph->t0;
        float s1 = glyph->s1;
        float t1 = glyph->t1;

        SET_GLYPH_VERTEX(vertices[vcount + 0], (float) (int) x0, y0, s0, t0,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 1], (float) (int) x0, y1, s0, t1,
                         markup.foreground_color, x0 - ((int) x0), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 2], (float) (int) x1, y1, s1, t1,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        SET_GLYPH_VERTEX(vertices[vcount + 3], (float) (int) x1, y0, s1, t0,
                         markup.foreground_color, x1 - ((int) x1), gamma);
        indices[icount + 0] = vcount + 0;
        indices[icount + 1] = vcount + 1;
        indices[icount + 2] = vcount + 2;
        indices[icount + 3] = vcount + 0;
        indices[icount + 4] = vcount + 2;
        indices[icount + 5] = vcount + 3;
        vcount += 4;
        icount += 6;

        buffer.push_back(vertices, vcount, indices, icount);
        pen.x += glyph->advance_x * (1.0f + markup.spacing);
    }
}

void TextBuffer::align(vec2 &pen, enum Align alignment)
{
    if (ALIGN_LEFT == alignment)
    {
        return;
    }

    size_t total_items = buffer.items.size();
    if (line_start != total_items)
    {
        finish_line(pen, false);
    }

    auto self_left = bounds.left;
    auto self_right = bounds.left + bounds.width;
    auto self_center = (self_left + self_right) / 2;
    auto lines_count = lines.size();

    for (auto i = 0; i < lines_count; ++i)
    {
        size_t line_end;

        if (i + 1 < lines_count)
        {
            line_end = lines[i + 1].line_start;
        }
        else
        {
            line_end = buffer.items.size();
        }

        auto& line = lines[i];
        auto line_right = line.bounds.left + line.bounds.width;

        float dx;
        if (ALIGN_RIGHT == alignment)
        {
            dx = self_right - line_right;
        } else // ALIGN_CENTER
        {
            line_left = line.bounds.left;
            auto line_center = (line_left + line_right) / 2;
            dx = self_center - line_center;
        }

        dx = roundf(dx);

        for (auto j = line.line_start; j < line_end; ++j)
        {
            auto& item = buffer.items[j];
            for (auto k = item.vstart; k < item.vstart + item.vcount; ++k)
            {
                buffer.vertices[k].x += dx;
            }
        }
    }
}

vec4 TextBuffer::get_bounds(vec2 &pen)
{
    size_t total_items = buffer.items.size();
    if (line_start != total_items)
    {
        finish_line(pen, false);
    }

    return bounds;
}

void TextBuffer::clear()
{
    buffer.clear();
    lines.clear();
    line_start = 0;
    line_ascender = 0;
    line_descender = 0;
    bounds = vec4{};
}

void TextBuffer::finish_line(vec2 &pen, bool advancePen)
{
    float line_left = this->line_left;
    float line_right = pen.x;
    float line_width = line_right - line_left;
    float line_top = pen.y + line_ascender;
    float line_height = line_ascender - line_descender;
    float line_bottom = line_top - line_height;

    {
        LineInfo line_info{};
        line_info.line_start = line_start;
        line_info.bounds.left = line_left;
        line_info.bounds.top = line_top;
        line_info.bounds.width = line_width;
        line_info.bounds.height = line_height;
        lines.push_back(line_info);
    }

    if (line_left < bounds.left)
    {
        bounds.left = line_left;
    }
    if (line_top > bounds.top)
    {
        bounds.top = line_top;
    }

    float self_right = bounds.left + bounds.width;
    float self_bottom = bounds.top - bounds.height;

    if (line_right > self_right)
    {
        bounds.width = line_right - bounds.left;
    }
    if (line_bottom < self_bottom)
    {
        bounds.height = bounds.top - line_bottom;
    }

    if (advancePen)
    {
        pen.x = origin.x;
        pen.y += (int) line_descender;
    }

    line_descender = 0;
    line_ascender = 0;
    line_start = buffer.items.size();
    line_left = pen.x;
}

void TextBuffer::move_last_line(float dy)
{
    for (auto& item: buffer.items)
    {
        for (auto i = item.vstart; i < item.vstart + item.vcount; ++i)
        {
            buffer.vertices[i].y -= dy;
        }
    }
}

}