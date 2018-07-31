/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include <freetype-gl.hpp>
#include <limits>
#include <string>
#include <memory>

namespace glez::detail::font
{

class font
{
public:
    ~font();

    bool load(const std::string &path, float size);
    bool loadFamily(std::string family, float size, bool bold, bool italic);

    void stringSize(const std::string &string, float *width, float *height);

    bool error{ false };
    std::unique_ptr<ftgl::TextureFont> texture{};
protected:
    bool internalLoad(ftgl::TextureFont *font);
};

}