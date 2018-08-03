/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#include <glez/detail/program.hpp>
#include <glez/detail/render.hpp>
#include <glez/detail/texture.hpp>
#include <glez/record.hpp>
#include <glez/glez.hpp>

namespace glez
{

void init(int width, int height)
{
    detail::program::init(width, height);
    detail::texture::init();
}

void shutdown()
{
    detail::program::shutdown();
    detail::texture::shutdown();
}

void resize(int width, int height)
{
    detail::program::resize(width, height);
}

void begin()
{
    detail::render::begin();
}

void end()
{
    detail::render::end();
}

}