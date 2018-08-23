/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include <glez/Render.hpp>
#include "glez/detail/freetype-gl/vertex-buffer.hpp"

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

void end();

GLuint shaderIdentity();
GLuint circleShader();

std::pair<int, int> getScreenSize();

};