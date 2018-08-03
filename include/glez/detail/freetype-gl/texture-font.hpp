/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "texture-atlas.hpp"

typedef struct FT_LibraryRec_ *FT_Library;
typedef struct FT_FaceRec_ *FT_Face;

namespace ftgl
{

/**
 * @file   texture-font.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup texture-font Texture font
 *
 * Texture font.
 *
 * Example Usage:
 * @code
 * #include "texture-font.h"
 *
 * int main( int arrgc, char *argv[] )
 * {
 *   return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * A list of possible ways to render a glyph.
 */
enum rendermode_t
{
    RENDER_NORMAL,
    RENDER_OUTLINE_EDGE,
    RENDER_OUTLINE_POSITIVE,
    RENDER_OUTLINE_NEGATIVE
};

/**
 * A structure that hold a kerning value relatively to a Unicode
 * codepoint.
 *
 * This structure cannot be used alone since the (necessary) right
 * Unicode codepoint is implicitely held by the owner of this structure.
 */
struct kerning_t
{
    /**
     * Left Unicode codepoint in the kern pair in UTF-32 LE encoding.
     */
    uint32_t codepoint;

    /**
     * Kerning value (in fractional pixels).
     */
    float kerning;

};

/*
 * Glyph metrics:
 * --------------
 *
 *                       xmin                     xmax
 *                        |                         |
 *                        |<-------- width -------->|
 *                        |                         |
 *              |         +-------------------------+----------------- ymax
 *              |         |    ggggggggg   ggggg    |     ^        ^
 *              |         |   g:::::::::ggg::::g    |     |        |
 *              |         |  g:::::::::::::::::g    |     |        |
 *              |         | g::::::ggggg::::::gg    |     |        |
 *              |         | g:::::g     g:::::g     |     |        |
 *    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    |
 *              |         | g:::::g     g:::::g     |     |        |
 *              |         | g::::::g    g:::::g     |     |        |
 *              |         | g:::::::ggggg:::::g     |     |        |
 *              |         |  g::::::::::::::::g     |     |      height
 *              |         |   gg::::::::::::::g     |     |        |
 *  baseline ---*---------|---- gggggggg::::::g-----*--------      |
 *            / |         |             g:::::g     |              |
 *     origin   |         | gggggg      g:::::g     |              |
 *              |         | g:::::gg   gg:::::g     |              |
 *              |         |  g::::::ggg:::::::g     |              |
 *              |         |   gg:::::::::::::g      |              |
 *              |         |     ggg::::::ggg        |              |
 *              |         |         gggggg          |              v
 *              |         +-------------------------+----------------- ymin
 *              |                                   |
 *              |------------- advance_x ---------->|
 */

/**
 * A structure that describe a glyph.
 */
class TextureGlyph
{
public:
    /**
     * Get the kerning between two horizontal glyphs.
     *
     * @param codepoint Character codepoint of the peceding character in UTF-8
     * encoding.
     *
     * @return x kerning value
     */
    float get_kerning(const char *codepoint) const;

    /**
     * Unicode codepoint this glyph represents in UTF-32 LE encoding.
     */
    uint32_t codepoint{ 0xFFFFFFFF };

    /**
     * Glyph's width in pixels.
     */
    size_t width{ 0 };

    /**
     * Glyph's height in pixels.
     */
    size_t height{ 0 };

    /**
     * Glyph's left bearing expressed in integer pixels.
     */
    int offset_x{ 0 };

    /**
     * Glyphs's top bearing expressed in integer pixels.
     *
     * Remember that this is the distance from the baseline to the top-most
     * glyph scanline, upwards y coordinates being positive.
     */
    int offset_y{ 0 };

    /**
     * For horizontal text layouts, this is the horizontal distance (in
     * fractional pixels) used to increment the pen position when the glyph is
     * drawn as part of a string of text.
     */
    float advance_x{ 0.0f };

    /**
     * For vertical text layouts, this is the vertical distance (in fractional
     * pixels) used to increment the pen position when the glyph is drawn as
     * part of a string of text.
     */
    float advance_y{ 0.0f };

    /**
     * First normalized texture coordinate (x) of top-left corner
     */
    float s0{ 0.0f };

    /**
     * Second normalized texture coordinate (y) of top-left corner
     */
    float t0{ 0.0f };

    /**
     * First normalized texture coordinate (x) of bottom-right corner
     */
    float s1{ 0.0f };

    /**
     * Second normalized texture coordinate (y) of bottom-right corner
     */
    float t1{ 0.0f };

    /**
     * A vector of kerning pairs relative to this glyph.
     */
    std::vector<kerning_t> kerning{};

    /**
     * Mode this glyph was rendered
     */
    rendermode_t rendermode{ RENDER_NORMAL };

    /**
     * Glyph outline thickness
     */
    float outline_thickness{ 0.0f };

};

/**
 *  Texture font structure.
 */
class TextureFont
{
public:
    /**
     * This function creates a new texture font from given filename and size.  The
     * texture atlas is used to store glyph on demand. Note the depth of the atlas
     * will determine if the font is rendered as alpha channel only (depth = 1) or
     * RGB (depth = 3) that correspond to subpixel rendering (if available on your
     * freetype implementation).
     *
     * @param atlas     A texture atlas
     * @param pt_size   Size of font to be created (in points)
     * @param filename  A font filename
     *
     */
    TextureFont(TextureAtlas& atlas, float pt_size, std::string filename);

    /**
     * This function creates a new texture font from a memory location and size.
     * The texture atlas is used to store glyph on demand. Note the depth of the
     * atlas will determine if the font is rendered as alpha channel only
     * (depth = 1) or RGB (depth = 3) that correspond to subpixel rendering (if
     * available on your freetype implementation).
     *
     * @param atlas       A texture atlas
     * @param pt_size     Size of font to be created (in points)
     * @param memory_base Start of the font file in memory
     * @param memory_size Size of the font file memory region, in bytes
     *
     */
    TextureFont(TextureAtlas& atlas, float pt_size, const void *memory_base, size_t memory_size);

    bool init();

    FT_Face load_face(float size);

    void generate_kerning(FT_Face face);

    /**
     * Request a new glyph from the font. If it has not been created yet, it will
     * be.
     *
     * @param codepoint Character codepoint to be loaded in UTF-8 encoding.
     *
     * @return A pointer on the new glyph or 0 if the texture atlas is not big
     *         enough
     *
     */
    TextureGlyph *get_glyph(const char *codepoint);

    /**
     * Request an already loaded glyph from the font.
     *
     * @param codepoint Character codepoint to be found in UTF-8 encoding.
     *
     * @return A pointer on the glyph or 0 if the glyph is not loaded
     */
    TextureGlyph *find_glyph(const char *codepoint);

    /**
     * Request the loading of a given glyph.
     *
     * @param codepoints Character codepoint to be loaded in UTF-8 encoding.
     *
     * @return One if the glyph could be loaded, zero if not.
     */
    bool load_glyph(const char *codepoint);

    /**
     * Request the loading of several glyphs at once.
     *
     * @param codepoints Character codepoints to be loaded in UTF-8 encoding. May
     *                   contain duplicates.
     *
     * @return Number of missed glyph if the texture is not big enough to hold
     *         every glyphs.
     */
    size_t load_glyphs(const char *codepoints);

    /**
     * Increases the size of a fonts texture atlas
     * Invalidates all pointers to font->atlas->data
     * Changes the UV Coordinates of existing glyphs in the font
     *
     * @param self A valid texture font
     * @param width_new Width of the texture atlas after resizing (must be bigger or
     *      equal to current width)
     * @param height_new Height of the texture atlas after resizing (must be bigger
     *      or equal to current height)
     */
    void enlarge_atlas(size_t width_new, size_t height_new);

    /**
     * Atlas structure to store glyphs data.
     */
    TextureAtlas& atlas;

    /**
     * Vector of glyphs contained in this font.
     */
    std::vector<TextureGlyph> glyphs{};

    /**
     * font location
     */
    enum
    {
        TEXTURE_FONT_FILE = 0,
        TEXTURE_FONT_MEMORY,
    } location;

    std::string filename{};

    struct
    {
        const void *base{ nullptr };
        size_t size{ 0 };
    } memory;

    /**
     * Font size
     */
    const float size;

    /**
     * Whether to use autohint when rendering font
     */
    bool hinting{ true };

    /**
     * Mode the font is rendering its next glyph
     */
    rendermode_t rendermode{ RENDER_NORMAL };

    /**
     * Outline thickness
     */
    float outline_thickness{ 0.0f };

    /**
     * Whether to use our own lcd filter.
     */
    bool filtering{ true };

    /**
     * LCD filter weights
     */
    unsigned char lcd_weights[5];

    /**
     * Whether to use kerning if available
     */
    bool kerning{ true };

    /**
     * This field is simply used to compute a default line spacing (i.e., the
     * baseline-to-baseline distance) when writing text with this font. Note
     * that it usually is larger than the sum of the ascender and descender
     * taken as absolute values. There is also no guarantee that no glyphs
     * extend above or below subsequent baselines when using this distance.
     */
    float height{ 0.0f };

    /**
     * This field is the distance that must be placed between two lines of
     * text. The baseline-to-baseline distance should be computed as:
     * ascender - descender + linegap
     */
    float linegap{ 0.0f };

    /**
     * The ascender is the vertical distance from the horizontal baseline to
     * the highest 'character' coordinate in a font face. Unfortunately, font
     * formats define the ascender differently. For some, it represents the
     * ascent of all capital latin characters (without accents), for others it
     * is the ascent of the highest accented character, and finally, other
     * formats define it as being equal to bbox.yMax.
     */
    float ascender{ 0.0f };

    /**
     * The descender is the vertical distance from the horizontal baseline to
     * the lowest 'character' coordinate in a font face. Unfortunately, font
     * formats define the descender differently. For some, it represents the
     * descent of all capital latin characters (without accents), for others it
     * is the ascent of the lowest accented character, and finally, other
     * formats define it as being equal to bbox.yMin. This field is negative
     * for values below the baseline.
     */
    float descender{ 0.0f };

    /**
     * The position of the underline line for this face. It is the center of
     * the underlining stem. Only relevant for scalable formats.
     */
    float underline_position{ 0.0f };

    /**
     * The thickness of the underline for this face. Only relevant for scalable
     * formats.
     */
    float underline_thickness;
};





/** @} */

}
