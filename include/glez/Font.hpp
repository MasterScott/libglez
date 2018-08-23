/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include <string>
#include <limits>
#include <memory>
#include <functional>
#include <glez/detail/freetype-gl/markup.hpp>

#include "detail/freetype-gl/texture-font.hpp"

namespace glez
{

/**
 * A font wrapper class
 */
class Font
{
public:
    /**
     * Create a font from a file
     */
    Font(std::string filename, float size);

    /**
     * Create a font based on markup
     */
    Font(ftgl::Markup markup, float size);

    /**
     * Create a font from description
     */
    Font(std::string family, float size, bool bold, bool italic);

    /**
     * Destroys the font
     * FIXME: Implement: actually remove the font from manager (needs ref counting: manager can return duplicate pointers)
     */
    ~Font() = default;

    /**
     * Get bounds of string
     */
    ftgl::vec2 getStringSize(const std::string& string);

    /**
     * Prepare the font for rendering
     * Loads, binds texture, etc
     */
    void prepare();

    /**
     * Load the font
     *
     * @return true if loading succeeded, false otherwise
     */
    bool load();

    /**
     * The size of the font in points
     */
    const float size;

    /**
     * Callback to load the font
     */
    std::function<ftgl::TextureFont *(void)> loader;

    /**
     * A pointer to internal font structure
     */
    ftgl::TextureFont *font{ nullptr };
};

}