/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include "texture-font.hpp"
#include "vec234.hpp"

namespace ftgl
{

/**
 * @file   markup.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup markup Markup
 *
 * Simple structure that describes text properties.
 *
 * <b>Example Usage</b>:
 * @code
 * #include "markup.h"
 *
 * ...
 *
 * vec4 black  = {{0.0, 0.0, 0.0, 1.0}};
 * vec4 white  = {{1.0, 1.0, 1.0, 1.0}};
 * vec4 none   = {{1.0, 1.0, 1.0, 0.0}};
 *
 * markup_t normal = {
 *     .family  = "Droid Serif",
 *     .size = 24.0,
 *     .bold = 0,
 *     .italic = 0,
 *     .spacing = 1.0,
 *     .gamma = 1.0,
 *     .foreground_color = black, .background_color    = none,
 *     .underline        = 0,     .underline_color     = black,
 *     .overline         = 0,     .overline_color      = black,
 *     .strikethrough    = 0,     .strikethrough_color = black,
 *     .font = 0,
 * };
 *
 * ...
 *
 * @endcode
 *
 * @{
 */

/**
 * Simple structure that describes text properties.
 */
struct Markup
{
    /**
     * A font family name such as "normal", "sans", "serif" or "monospace".
     */
    std::string family{};

    /**
     * Font size.
     */
    float size;

    /**
     * Whether text is bold.
     */
    bool bold{ false };

    /**
     * Whether text is italic.
     */
    bool italic{ false };

    /**
     * Spacing between letters.
     */
    float spacing{ 1.0f };

    /**
     * Gamma correction.
     */
    float gamma{ 1.0f };

    /**
     * Text color.
     */
    vec4 foreground_color{};

    /**
     * Background color.
     */
    vec4 background_color{};

    /**
     * Whether outline is active.
     */
    bool outline{ false };

    /**
     * Outline color.
     */
    vec4 outline_color{};

    /**
     * Whether underline is active.
     */
    bool underline{ false };

    /**
     * Underline color.
     */
    vec4 underline_color{};

    /**
     * Whether overline is active.
     */
    bool overline{ false };

    /**
     * Overline color.
     */
    vec4 overline_color{};

    /**
     * Whether strikethrough is active.
     */
    bool strikethrough{ false };

    /**
     * Strikethrough color.
     */
    vec4 strikethrough_color{};

    /**
     * Pointer on the corresponding font (family/size/bold/italic)
     */
    TextureFont *font{ nullptr };

};

/** @} */

}