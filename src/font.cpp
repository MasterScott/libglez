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
    switch (source.mode)
    {
    case source_type::mode_enum::FAMILY:
        loaded = pointer->loadFamily(source.string, size, source.bold, source.italic);
        break;
    case source_type::mode_enum::PATH:
        loaded = pointer->load(source.string, size);
        break;
    }
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
    source.mode = source.PATH;
    source.string = path;
}

font::font(std::string family, float size, bool bold, bool italic): size(size)
{
    pointer = std::make_unique<detail::font::font>();
    source.mode = source.FAMILY;
    source.bold = bold;
    source.italic = italic;
}

}