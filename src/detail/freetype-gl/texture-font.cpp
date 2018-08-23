/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
// #include FT_ADVANCES_H
#include FT_LCD_FILTER_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "glez/detail/freetype-gl/texture-font.hpp"
#include "glez/detail/freetype-gl/utf8-utils.hpp"
#include <stdexcept>

#define HRES 64
#define HRESf 64.f
#define DPI 72

#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s) { e, s },
#define FT_ERROR_START_LIST {
#define FT_ERROR_END_LIST                                                      \
    {                                                                          \
        0, 0                                                                   \
    }                                                                          \
    }                                                                          \
    ;
const struct
{
    int code;
    const char *message;
} FT_Errors[] =
#include FT_ERRORS_H

static FT_Library library{ nullptr };

class FreeTypeRAII
{
public:
    static size_t count;

    explicit FreeTypeRAII()
    {
        if (!count)
        {
            FT_Init_FreeType(&library);
        }
        ++count;
    }

    ~FreeTypeRAII()
    {
        if (count == 1)
        {
            FT_Done_FreeType(library);
        }
        --count;
    }
};

size_t FreeTypeRAII::count = 0;

class FaceRAII
{
public:
    explicit FaceRAII(FT_Face face): face(face)
    {
    }

    ~FaceRAII()
    {
        if (face != nullptr)
            FT_Done_Face(face);
    }

    inline FT_Face operator*() const
    {
        return face;
    }

    FT_Face face{ nullptr };
};

class StrokerRAII
{
public:
    StrokerRAII()
    {
        FT_Stroker_New(library, &stroker);
    }

    ~StrokerRAII()
    {
        if (stroker != nullptr)
            FT_Stroker_Done(stroker);
    }

    inline FT_Stroker operator*() const
    {
        return stroker;
    }

    FT_Stroker stroker{ nullptr };
};

namespace ftgl
{

float TextureGlyph::get_kerning(const char *codepoint) const
{
    auto ucodepoint = utf8_to_utf32(codepoint);

    for (auto& k: kerning)
    {
        if (k.codepoint == ucodepoint)
        {
            return k.kerning;
        }
    }
    return 0;
}

TextureFont::TextureFont(TextureAtlas &atlas, float pt_size,
                         std::string filename): atlas(atlas), size(pt_size), filename(std::move(filename)), location(TEXTURE_FONT_FILE)
{
}

TextureFont::TextureFont(TextureAtlas &atlas, float pt_size,
                         const void *memory_base, size_t memory_size): atlas(atlas), size(pt_size), location(TEXTURE_FONT_MEMORY), memory({ memory_base, memory_size })
{
    assert(memory_base);
    assert(memory_size);
}

TextureGlyph *TextureFont::get_glyph(const char *codepoint)
{
    TextureGlyph *glyph;

    /* Check if codepoint has been already loaded */
    if ((glyph = find_glyph(codepoint)))
        return glyph;

    /* Glyph has not been already loaded */
    if (load_glyph(codepoint))
        return find_glyph(codepoint);

    return nullptr;
}

TextureGlyph *TextureFont::find_glyph(const char *codepoint)
{
    uint32_t ucodepoint = utf8_to_utf32(codepoint);

    for (auto& glyph: glyphs)
    {
        // If codepoint is -1, we don't care about outline type or thickness
        if ((glyph.codepoint == ucodepoint) &&
            ((ucodepoint == -1) ||
             ((glyph.rendermode == rendermode) &&
              (glyph.outline_thickness == outline_thickness))))
        {
            return &glyph;
        }
    }

    return nullptr;
}

bool TextureFont::load_glyph(const char *codepoint)
{
    FreeTypeRAII ft_raii{};

    FaceRAII face(load_face(size));
    if (*face == nullptr)
        return false;

    /* Check if codepoint has been already loaded */
    if (find_glyph(codepoint))
        return true;

    /* codepoint NULL is special : it is used for line drawing (overline,
     * underline, strikethrough) and background.
     */
    if (!codepoint)
    {
        auto region = atlas.get_region(5, 5);
        unsigned char data[4 * 4 * atlas.depth];
        memset(data, 255, sizeof(data));
        if (region.x < 0)
        {
            fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
            return false;
        }
        atlas.set_region((size_t)region.x, (size_t)region.y, 4, 4, data, 0);

        TextureGlyph glyph{};
        glyph.codepoint = 0xFFFFFFFF;
        glyph.s0 = (region.x + 2) / (float) atlas.width;
        glyph.t0 = (region.y + 2) / (float) atlas.height;
        glyph.s1 = (region.x + 3) / (float) atlas.width;
        glyph.t1 = (region.y + 3) / (float) atlas.height;
        glyphs.push_back(glyph);

        return true;
    }

    FT_Int32 flags{ 0 };
    int ft_glyph_top{ 0 };
    int ft_glyph_left{ 0 };

    auto glyph_index = FT_Get_Char_Index(*face, (FT_ULong) utf8_to_utf32(codepoint));

    // WARNING: We use texture-atlas depth to guess if user wants
    //          LCD subpixel rendering

    if (rendermode != RENDER_NORMAL)
        flags |= FT_LOAD_NO_BITMAP;
    else
        flags |= FT_LOAD_RENDER;

    if (!hinting)
        flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
    else
        flags |= FT_LOAD_FORCE_AUTOHINT;

    if (atlas.depth == 3)
    {
        FT_Library_SetLcdFilter(library, FT_LCD_FILTER_LIGHT);
        flags |= FT_LOAD_TARGET_LCD;

        if (filtering)
            FT_Library_SetLcdFilterWeights(library, lcd_weights);
    }

    auto error = FT_Load_Glyph(*face, glyph_index, flags);
    if (error)
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n", __LINE__,
                FT_Errors[error].code, FT_Errors[error].message);
        return false;
    }

    FT_Glyph ft_glyph;
    FT_Bitmap ft_bitmap;

    if (rendermode == RENDER_NORMAL)
    {
        auto slot = (*face)->glyph;
        ft_bitmap = slot->bitmap;
        ft_glyph_top = slot->bitmap_top;
        ft_glyph_left = slot->bitmap_left;
    } else
    {
        StrokerRAII stroker{};
        FT_BitmapGlyph ft_bitmap_glyph;

        if (*stroker == nullptr)
            return false;

        FT_Stroker_Set(*stroker, (int) (outline_thickness * HRES),
                       FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

        error = FT_Get_Glyph((*face)->glyph, &ft_glyph);

        if (error)
        {
            fprintf(stderr, "FT_Error (0x%02x) : %s\n", FT_Errors[error].code,
                    FT_Errors[error].message);
            return false;
        }

        switch (rendermode)
        {
        case RENDER_OUTLINE_EDGE:
            error = FT_Glyph_Stroke(&ft_glyph, *stroker, 1);
            break;
        case RENDER_OUTLINE_POSITIVE:
            error = FT_Glyph_StrokeBorder(&ft_glyph, *stroker, 0, 1);
            break;
        case RENDER_OUTLINE_NEGATIVE:
            error = FT_Glyph_StrokeBorder(&ft_glyph, *stroker, 1, 1);
            break;
        default:
            break;
        }

        if (error)
        {
            fprintf(stderr, "FT_Error (0x%02x) : %s\n", FT_Errors[error].code,
                    FT_Errors[error].message);
            return false;
        }

        if (atlas.depth == 1)
            error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
        else
            error = FT_Glyph_To_Bitmap(&ft_glyph, FT_RENDER_MODE_LCD, 0, 1);

        if (error)
        {
            fprintf(stderr, "FT_Error (0x%02x) : %s\n", FT_Errors[error].code,
                    FT_Errors[error].message);
            return false;
        }

        ft_bitmap_glyph = (FT_BitmapGlyph) ft_glyph;
        ft_bitmap = ft_bitmap_glyph->bitmap;
        ft_glyph_top = ft_bitmap_glyph->top;
        ft_glyph_left = ft_bitmap_glyph->left;
    }

    struct
    {
        int left;
        int top;
        int right;
        int bottom;
    } padding = {0, 0, 1, 1};

    size_t src_w = ft_bitmap.width / atlas.depth;
    size_t src_h = ft_bitmap.rows;

    size_t tgt_w = src_w + padding.left + padding.right;
    size_t tgt_h = src_h + padding.top + padding.bottom;

    auto region = atlas.get_region(tgt_w, tgt_h);

    if (region.x < 0)
    {
        fprintf(stderr, "Texture atlas is full (line %d)\n", __LINE__);
        return false;
    }

    auto x = region.x;
    auto y = region.y;

    auto buffer = new unsigned char[tgt_w * tgt_h * atlas.depth];
    memset(buffer, 0, tgt_w * tgt_h * atlas.depth);
    auto dst_ptr = &buffer[(padding.top * tgt_w + padding.left) * atlas.depth];
    auto src_ptr = ft_bitmap.buffer;
    for (auto i = 0; i < src_h; i++)
    {
        // difference between width and pitch:
        // https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_Bitmap
        memcpy(dst_ptr, src_ptr, ft_bitmap.width);
        dst_ptr += tgt_w * atlas.depth;
        src_ptr += ft_bitmap.pitch;
    }

    atlas.set_region((size_t)x, (size_t)y, tgt_w, tgt_h, buffer, tgt_w * atlas.depth);

    delete[] buffer;

    TextureGlyph glyph{};

    glyph.codepoint = utf8_to_utf32(codepoint);
    glyph.width = tgt_w;
    glyph.height = tgt_h;
    glyph.rendermode = rendermode;
    glyph.outline_thickness = outline_thickness;
    glyph.offset_x = ft_glyph_left;
    glyph.offset_y = ft_glyph_top;
    glyph.s0 = x / (float) atlas.width;
    glyph.t0 = y / (float) atlas.height;
    glyph.s1 = (x + glyph.width) / (float) atlas.width;
    glyph.t1 = (y + glyph.height) / (float) atlas.height;

    // Discard hinting to get advance
    FT_Load_Glyph(*face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
    auto slot = (*face)->glyph;
    glyph.advance_x = slot->advance.x / HRESf;
    glyph.advance_y = slot->advance.y / HRESf;

    glyphs.push_back(std::move(glyph));

    if (rendermode != RENDER_NORMAL)
        FT_Done_Glyph(ft_glyph);

    generate_kerning(*face);

    return true;
}

size_t TextureFont::load_glyphs(const char *codepoints)
{
    /* Load each glyph */
    for (auto i = 0; i < strlen(codepoints); i += utf8_surrogate_len(codepoints + i))
    {
        if (!load_glyph(codepoints + i))
            return utf8_strlen(codepoints + i);
    }

    return 0;
}

void TextureFont::enlarge_atlas(size_t width_new, size_t height_new)
{
    // ensure size increased
    assert(width_new >= atlas.width);
    assert(height_new >= atlas.height);
    assert(width_new + height_new > atlas.width + atlas.height);

    size_t width_old = atlas.width;
    size_t height_old = atlas.height;
    // allocate new buffer
    unsigned char *data_old = atlas.data;
    atlas.data = new unsigned char[width_new * height_new * atlas.depth];
    memset(atlas.data, 0, width_new * height_new * atlas.depth);
    // update atlas size
    atlas.width = width_new;
    atlas.height = height_new;
    // add node reflecting the gained space on the right
    if (width_new > width_old)
    {
        atlas.nodes.emplace_back(ivec3 {(int)width_old - 1, 1, (int)width_new - (int)width_old});
    }
    // copy over data from the old buffer, skipping first row and column because
    // of the margin
    size_t pixel_size = sizeof(char) * atlas.depth;
    size_t old_row_size = width_old * pixel_size;
    atlas.set_region(1, 1, width_old - 2, height_old - 2,
                             data_old + old_row_size + pixel_size,
                             old_row_size);

    delete[] data_old;

    // change uv coordinates of existing glyphs to reflect size change
    float mulw = (float) width_old / width_new;
    float mulh = (float) height_old / height_new;
    size_t i;
    for (auto& g: glyphs)
    {
        g.s0 *= mulw;
        g.s1 *= mulw;
        g.t0 *= mulh;
        g.t1 *= mulh;
    }
}

bool TextureFont::init()
{
    FreeTypeRAII ft_raii{};

    if (size <= 0)
        return false;

    if (location == TEXTURE_FONT_FILE && filename.empty())
        return false;
    if (location == TEXTURE_FONT_MEMORY && !(memory.base && memory.size))
        return false;

    // FT_LCD_FILTER_LIGHT   is (0x00, 0x55, 0x56, 0x55, 0x00)
    // FT_LCD_FILTER_DEFAULT is (0x10, 0x40, 0x70, 0x40, 0x10)
    lcd_weights[0] = 0x10;
    lcd_weights[1] = 0x40;
    lcd_weights[2] = 0x70;
    lcd_weights[3] = 0x40;
    lcd_weights[4] = 0x10;

    auto face = load_face(size * 100.f);
    FT_Size_Metrics metrics;

    if (!face)
        return false;

    underline_position =
            face->underline_position / (float) (HRESf * HRESf) * size;
    underline_position = roundf(underline_position);
    if (underline_position > -2)
    {
        underline_position = -2.0f;
    }

    underline_thickness =
            face->underline_thickness / (float) (HRESf * HRESf) * size;
    underline_thickness = roundf(underline_thickness);
    if (underline_thickness < 1)
    {
        underline_thickness = 1.0;
    }

    metrics = face->size->metrics;
    ascender = (metrics.ascender >> 6) / 100.0;
    descender = (metrics.descender >> 6) / 100.0;
    height = (metrics.height >> 6) / 100.0;
    linegap = height - ascender + descender;
    FT_Done_Face(face);

    /* NULL is a special glyph */
    get_glyph(nullptr);

    return true;
}

FT_Face TextureFont::load_face(float size)
{
    FreeTypeRAII ft_raii{};

    FT_Matrix matrix = {(int) ((1.0 / HRES) * 0x10000L),
                        (int) ((0.0) * 0x10000L), (int) ((0.0) * 0x10000L),
                        (int) ((1.0) * 0x10000L)};

    assert(size);

    FT_Error error;
    FT_Face face{};

    /* Load face */
    switch (location)
    {
    case TEXTURE_FONT_FILE:
        error = FT_New_Face(library, filename.c_str(), 0, &face);
        break;
    case TEXTURE_FONT_MEMORY:
        error = FT_New_Memory_Face(library, (const FT_Byte *)memory.base,
                                   memory.size, 0, &face);
        break;
    }

    FaceRAII face_raii{ face };

    if (error)
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n", __LINE__,
                FT_Errors[error].code, FT_Errors[error].message);
        return nullptr;
    }

    /* Select charmap */
    error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
    if (error)
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n", __LINE__,
                FT_Errors[error].code, FT_Errors[error].message);
        return nullptr;
    }

    /* Set char size */
    error = FT_Set_Char_Size(face, (int) (size * HRES), 0, DPI * HRES, DPI);

    if (error)
    {
        fprintf(stderr, "FT_Error (line %d, code 0x%02x) : %s\n", __LINE__,
                FT_Errors[error].code, FT_Errors[error].message);
        return nullptr;
    }

    /* Set transform matrix */
    FT_Set_Transform(face, &matrix, nullptr);

    /* Prevent the deletion */
    face_raii.face = nullptr;

    return face;
}

void
TextureFont::generate_kerning(FT_Face face)
{
    FT_UInt prev_index;
    FT_Vector kerning;

    /* For each glyph couple combination, check if kerning is necessary */
    /* Starts at index 1 since 0 is for the special backgroudn glyph */
    for (auto i = 1; i < glyphs.size(); ++i)
    {
        auto& glyph = glyphs[i];
        auto glyph_index = FT_Get_Char_Index(face, glyph.codepoint);
        glyph.kerning.clear();

        for (auto j = 1; j < glyphs.size(); ++j)
        {
            auto& prev_glyph = glyphs[j];
            prev_index = FT_Get_Char_Index(face, prev_glyph.codepoint);
            FT_Get_Kerning(face, prev_index, glyph_index, FT_KERNING_UNFITTED,
                           &kerning);
            if (kerning.x)
            {
                glyph.kerning.push_back({prev_glyph.codepoint,
                                         kerning.x / (HRESf * HRESf)});
            }
        }
    }
}

}