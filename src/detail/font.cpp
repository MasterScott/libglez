/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/detail/font.hpp>
#include <vector>
#include <memory>
#include <cassert>
#include <glez/detail/freetype-gl/font-manager.hpp>

namespace glez::detail::font
{

void font::stringSize(const std::string &string, float *width, float *height)
{
    if (!texture)
        return;

    float penX = 0;

    float size_x = 0;
    float size_y = 0;

    texture->load_glyphs(string.c_str());

    const char *sstring = string.c_str();

    for (size_t i = 0; i < string.size(); ++i)
    {
        // c_str guarantees a NULL terminator
        auto glyph = texture->find_glyph(&sstring[i]);
        if (glyph == nullptr)
            continue;

        penX += glyph->get_kerning(&sstring[i]);
        penX += glyph->advance_x;

        if (penX > size_x)
            size_x = penX;

        if (glyph->height > size_y)
            size_y = glyph->height;
    }
    if (width)
        *width = size_x;
    if (height)
        *height = size_y;
}

bool font::internalLoad(ftgl::TextureFont *font)
{
    if (font == nullptr)
    {
        error = true;
        return false;
    }
    texture.reset(font);
    return true;
}

}