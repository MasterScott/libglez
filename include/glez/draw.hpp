/*
  Created by Jenny White on 30.04.18.
  Copyright (c) 2018 nullworks. All rights reserved.
*/

#pragma once

#include <string>
#include "color.hpp"
#include "Font.hpp"

namespace glez::draw
{

void line(float x, float y, float dx, float dy, rgba color, float thickness);
void rect(float x, float y, float w, float h, rgba color);
void rect_outline(float x, float y, float w, float h, rgba color, float thickness);
void circle(float x, float y, float radius, rgba color, float thickness, int steps);


void line(float x, float y, float dx, float dy, rgba color_begin, rgba color_end, float thickness);

void rect(float x, float y, float w, float h, rgba color_nw, rgba color_ne, rgba color_se, rgba color_sw);

void rect_outline(float x, float y, float w, float h, rgba color_nw, rgba color_ne, rgba color_se, rgba color_sw);

void circle(float x, float y, float radius_inner, float radius_outer, rgba color);


/*void rect_textured(float x, float y, float w, float h, rgba color, texture &texture,
                   float tx, float ty, float tw, float th, float angle);*/
/*void string(float x, float y, const std::string &string, Font &font, rgba color,
            float *width, float *height);
void outlined_string(float x, float y, const std::string &string, Font &font,
                     rgba color, rgba outline, float *width, float *height);*/

}