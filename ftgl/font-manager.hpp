/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include "markup.hpp"
#include "texture-font.hpp"
#include "texture-atlas.hpp"

#include <vector>
#include <string>
#include <optional>
#include <memory>

namespace ftgl
{

/**
 * @file   font-manager.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup font-manager Font manager
 *
 * Structure in charge of caching fonts.
 *
 * <b>Example Usage</b>:
 * @code
 * #include "font-manager.h"
 *
 * int main( int arrgc, char *argv[] )
 * {
 *     font_manager_t * manager = manager_new( 512, 512, 1 );
 *     texture_font_t * font = font_manager_get( manager, "Mono", 12, 0, 0 );
 *
 *     return 0;
 * }
 * @endcode
 *
 * @{
 */

/**
 * Structure in charge of caching fonts.
 */
class FontManager
{
public:
    /**
     * Creates a new empty font manager.
     *
     * @param   width   width of the underlying atlas
     * @param   height  height of the underlying atlas
     * @param   depth   bit depth of the underlying atlas
     *
     */
    FontManager(size_t width, size_t height, size_t depth);

    /**
     *  Deletes a font from the font manager.
     *
     *  Note that font glyphs are not removed from the atlas.
     *
     *  @param font font to be deleted
     *
     */
    void delete_font(TextureFont& font);

    /**
     *  Request for a font based on a filename.
     *
     *  @param filename font filename
     *  @param size     font size
     *
     *  @return Requested font
     */
    TextureFont *get_from_filename(std::string filename, float size);

    /**
     *  Request for a font based on a description
     *
     *  @param self     a font manager
     *  @param family   font family
     *  @param size     font size
     *  @param bold     whether font is bold
     *  @param italic   whether font is italic
     *
     *  @return Requested font
     */
    TextureFont *get_from_description(std::string family, float size, bool bold, bool italic);

    /**
     *  Request for a font based on a markup
     *
     *  @param self    a font manager
     *  @param markup  Markup describing a font
     *
     *  @return Requested font
     */
    TextureFont *get_from_markup(const Markup& markup);

    /**
     *  Search for a font filename that match description.
     *
     *  @param self    a font manager
     *  @param family   font family
     *  @param size     font size
     *  @param bold     whether font is bold
     *  @param italic   whether font is italic
     *
     *  @return Requested font filename
     */
    std::optional<std::string> match_description(std::string family, float size, bool bold, bool italic);

    /**
     * Texture atlas to hold font glyphs.
     */
    TextureAtlas atlas;

    /**
     * Cached textures.
     */
    std::vector<std::unique_ptr<TextureFont>> fonts{};

    /**
     * Default glyphs to be loaded when loading a new font.
     */
    std::vector<char> cache{};

};









/** @} */

}
