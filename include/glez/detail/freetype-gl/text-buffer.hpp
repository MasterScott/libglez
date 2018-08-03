/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include "vertex-buffer.hpp"
#include "markup.hpp"

namespace ftgl
{

/**
 * Use LCD filtering
 */
#define LCD_FILTERING_ON 3

/**
 * Do not use LCD filtering
 */
#define LCD_FILTERING_OFF 1

/**
 * @file   text-buffer.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup text-buffer Text buffer
 *
 *
 * <b>Example Usage</b>:
 * @code
 *
 * int main( int arrgc, char *argv[] )
 * {
 *
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * Align enumeration
 */
enum Align
{
    /**
     * Align text to the left hand side
     */
            ALIGN_LEFT,

    /**
     * Align text to the center
     */
            ALIGN_CENTER,

    /**
     * Align text to the right hand side
     */
            ALIGN_RIGHT
};

/**
 * Line structure
 */
struct LineInfo
{
    /**
     * Index (in the vertex buffer) where this line starts
     */
    size_t line_start;

    /**
     * bounds of this line
     */
    vec4 bounds;

};

/**
 * Glyph vertex structure
 */
struct GlyphVertex
{
    /**
     * Vertex x coordinates
     */
    float x;

    /**
     * Vertex y coordinates
     */
    float y;

    /**
     * Texture first coordinate
     */
    float u;

    /**
     * Texture second coordinate
     */
    float v;

    vec4 color;

    /**
     * Shift along x
     */
    float shift;

    /**
     * Color gamma correction
     */
    float gamma;
};

/**
 * Text buffer structure
 */
class TextBuffer
{
    /**
     * Creates a new empty text buffer.
     */
    TextBuffer() = default;

    /**
     * Print some text to the text buffer
     *
     * @param pen  position of text start
     * @param ...  a series of markup_t *, char * ended by NULL
     *
     */
    void text_printf(vec2 *pen, ...);

    /**
     * Add some text to the text buffer
     *
     * @param pen    position of text start
     * @param markup Markup to be used to add text
     * @param text   Text to be added
     * @param length Length of text to be added
     */
    void add_text(vec2& pen, const Markup& markup, const char *text, size_t length);

    /**
     * Add a char to the text buffer
     *
     * @param pen      position of text start
     * @param markup   markup to be used to add text
     * @param current  charactr to be added
     * @param previous previous character (if any)
     */
    void add_char(vec2& pen, const Markup& markup, const char *current, const char *previous);

    /**
     * Align all the lines of text already added to the buffer
     * This alignment will be relative to the overall bounds of the
     * text which can be queried by text_buffer_get_bounds
     *
     * @param pen       pen used in last call (must be unmodified)
     * @param alignment desired alignment of text
     */
    void align(vec2& pen, enum Align alignment);

    /**
     * Get the rectangle surrounding the text
     *
     * @param pen       pen used in last call (must be unmodified)
     */
    vec4 get_bounds(vec2& pen);

    /**
     * Clear text buffer
     */
    void clear();

    void move_last_line(float dy);

    void finish_line(vec2 &pen, bool advancePen);

    /**
     * Vertex buffer
     */
    VertexBuffer<GlyphVertex> buffer{ "vertex:2f,tex_coord:2f,color:4f,ashift:1f,agamma:1f" };

    /**
     * Base color for text
     */
    vec4 base_color{ 0.0f, 0.0f, 0.0f, 1.0f };

    /**
     * Pen origin
     */
    vec2 origin{};

    /**
     * Last pen y location
     */
    float last_pen_y{ 0.0f };

    /**
     * Total bounds
     */
    vec4 bounds{};

    /**
     * Index (in the vertex buffer) of the current line start
     */
    size_t line_start{ 0 };

    /**
     * Location of the start of the line
     */
    float line_left{ 0.0f };

    /**
     * Vector of line information
     */
    std::vector<LineInfo> lines{};

    /**
     * Current line ascender
     */
    float line_ascender{ 0.0f };

    /**
     * Current line decender
     */
    float line_descender{ 0.0f };
};

/** @} */

}
