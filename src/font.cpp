/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/font.hpp>
#include <glez/detail/font.hpp>

namespace glez
{

void font::load()
{
    if (detail)
    {
        // BUGBUG, Already loaded
        return;
    }



    font.load(path, size);
    loaded = true;
}

void font::stringSize(const std::string &string, float *width, float *height)
{
    if (!isLoaded())
        load();
    if (!pointer)
        return;
    pointer->stringSize(string, width, height);
}

font::font(std::string path, float size): size(size)
{
    pointer = std::make_unique<detail::font::font>();
}

font::font(std::string family, float size, bool bold, bool italic): size(size)
{
    pointer = std::make_unique<detail::font::font>();

}

}