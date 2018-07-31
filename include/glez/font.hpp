/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include <string>
#include <limits>
#include <glez/detail_forward.hpp>
#include <memory>

namespace glez
{

class font
{
public:
    font(std::string path, float size);
    font(std::string family, float size, bool bold, bool italic);

    inline bool isLoaded() const
    {
        return !error && pointer;
    }

    void stringSize(const std::string &string, float *width, float *height);

    const float size;
protected:
    void load();

    bool error{ false };
    std::unique_ptr<detail::font::font> pointer{ nullptr };
};
}