/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/detail/font.hpp>
#include <vector>
#include <memory>
#include <cassert>
#include <font-manager.hpp>

static std::unique_ptr<ftgl::FontManager> manager{};
static bool init_done{ false };

static void init()
{
    manager = std::make_unique<ftgl::FontManager>(1024, 1024, 1);
    init_done = true;
}

namespace glez::detail::font
{

font::~font()
{
    if (texture)
    {
        manager->delete_font(*texture);
    }
    texture.reset(nullptr);
}

bool font::load(const std::string &path, float size)
{
    if (!init_done)
        init();
    return internalLoad(manager->get_from_filename(path, size));
}

bool font::loadFamily(std::string family, float size, bool bold, bool italic)
{
    if (!init_done)
        init();
    return internalLoad(manager->get_from_description(std::move(family), size, bold, italic));
}

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