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

/*
 * Rectangles
 * Lines
 * Circles
 * Text
 */

/**
 * Render a solid-colored filled rectangle
 * @param x X position on screen
 * @param y Y position on screen
 * @param w Width
 * @param h Height
 * @param color The color
 */
void rect(float x, float y, float w, float h, rgba color);

/**
 * Render a rectangle with color transition
 * @param x
 * @param y
 * @param w
 * @param h
 * @param color_nw Top left color
 * @param color_ne Top right color
 * @param color_se Bottom right color
 * @param color_sw Bottom left color
 */
void rect(float x, float y, float w, float h, rgba color_nw, rgba color_ne, rgba color_se, rgba color_sw);

void rect_outline(float x, float y, float w, float h, rgba color, float thickness);

void string(float x, float y, std::string_view string, Font& font, rgba color);

void outlined_string(float x, float y, std::string_view string, Font& font, rgba color, rgba outline);

/*void line(float x, float y, float dx, float dy, rgba color, float thickness);

void line(float x, float y, float dx, float dy, rgba color_begin, rgba color_end, float thickness);

void circle(float x, float y, float radius, rgba color, float thickness, int steps);



void rect(float x, float y, float w, float h, rgba color_nw, rgba color_ne, rgba color_se, rgba color_sw);

void rect_outline(float x, float y, float w, float h, rgba color_nw, rgba color_ne, rgba color_se, rgba color_sw);

void circle(float x, float y, float radius_inner, float radius_outer, rgba color);*/


/*void rect_textured(float x, float y, float w, float h, rgba color, texture &texture,
                   float tx, float ty, float tw, float th, float angle);*/
/*void string(float x, float y, const std::string &string, Font &font, rgba color,
            float *width, float *height);
void outlined_string(float x, float y, const std::string &string, Font &font,
                     rgba color, rgba outline, float *width, float *height);*/

}