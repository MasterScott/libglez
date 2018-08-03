/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include "glez/detail/freetype-gl/vertex-buffer.hpp"
#include "render.hpp"

namespace glez::detail::program
{

enum class mode
{
    PLAIN = 1,
    TEXTURED,
    FREETYPE
};

void init(int width, int height);
void shutdown();

void resize(int width, int height);

void draw();
void reset();

void begin();
void end();

unsigned next_index();

extern ftgl::VertexBuffer<glez::detail::render::vertex> buffer;
};