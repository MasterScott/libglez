/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#include <fontconfig/fontconfig.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "font-manager.hpp"

namespace ftgl
{

// ------------------------------------------------------------ file_exists ---
static int file_exists(const char *filename)
{
    auto file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

FontManager::FontManager(size_t width, size_t height, size_t depth): atlas(width, height, depth)
{
    cache.push_back(' ');
    cache.push_back('\0');
}

void FontManager::delete_font(TextureFont &font)
{
    for (auto it = fonts.begin(); it != fonts.end();)
    {
        if ((*it)->filename == font.filename && font.size == (*it)->size)
            it = fonts.erase(it);
        else
            ++it;
    }
}

TextureFont *FontManager::get_from_filename(std::string filename, float size)
{
    for (auto& font: fonts)
    {
        if ((font->filename == filename) && (font->size == size))
        {
            return font.get();
        }
    }
    auto font = std::make_unique<TextureFont>(atlas, size, filename);
    if (font->init())
    {
        auto result = font.get();
        font->load_glyphs(cache.data());
        fonts.push_back(std::move(font));
        return result;
    }
    fprintf(stderr, "Unable to load \"%s\" (size=%.1f)\n", filename.c_str(), size);
    return nullptr;
}

TextureFont *
FontManager::get_from_description(std::string family, float size, bool bold,
                                  bool italic)
{
    std::string filename{};

    if (file_exists(family.c_str()))
    {
        filename = family;
    }
    else
    {
        auto found = match_description(family, size, bold, italic);
        if (!found)
        {
            fprintf(
                    stderr,
                    "No \"%s (size=%.1f, bold=%d, italic=%d)\" font available.\n",
                    family.c_str(), size, bold, italic);
            return nullptr;
        }
    }

    return get_from_filename(filename, size);
}

TextureFont *FontManager::get_from_markup(const Markup &markup)
{
    return get_from_description(markup.family, markup.size, markup.bold, markup.italic);
}

std::optional<std::string>
FontManager::match_description(std::string family, float size, bool bold, bool italic)
{
    int weight     = FC_WEIGHT_REGULAR;
    int slant      = FC_SLANT_ROMAN;
    if (bold)
    {
        weight = FC_WEIGHT_BOLD;
    }
    if (italic)
    {
        slant = FC_SLANT_ITALIC;
    }

    FcInit();
    auto pattern = FcPatternCreate();
    FcPatternAddDouble(pattern, FC_SIZE, size);
    FcPatternAddInteger(pattern, FC_WEIGHT, weight);
    FcPatternAddInteger(pattern, FC_SLANT, slant);
    FcPatternAddString(pattern, FC_FAMILY, (FcChar8 *) family.c_str());
    FcConfigSubstitute(nullptr, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);
    FcResult result;
    auto match = FcFontMatch(nullptr, pattern, &result);
    FcPatternDestroy(pattern);

    if (!match)
    {
        fprintf(stderr, "fontconfig error: could not match family '%s'",
                family.c_str());
        return std::nullopt;
    }
    else
    {
        FcValue value;
        auto font = FcPatternGet(match, FC_FILE, 0, &value);
        if (font)
        {
            fprintf(stderr, "fontconfig error: could not match family '%s'",
                    family.c_str());
            FcPatternDestroy(match);
            return std::nullopt;
        }
        else
        {
            std::optional<std::string> path = std::string((const char *)value.u.s);
            FcPatternDestroy(match);
            return path;
        }
    }
}
}