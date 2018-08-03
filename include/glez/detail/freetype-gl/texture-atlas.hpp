/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 *  ============================================================================
 *
 *
 * This source is based on the article by Jukka Jylänki :
 * "A Thousand Ways to Pack the Bin - A Practical Approach to
 * Two-Dimensional Rectangle Bin Packing", February 27, 2010.
 *
 * More precisely, this is an implementation of the Skyline Bottom-Left
 * algorithm based on C++ sources provided by Jukka Jylänki at:
 * http://clb.demon.fi/files/RectangleBinPack/
 *
 *  ============================================================================
 */

#pragma once

#include <cstdlib>
#include <vector>

#include "vec234.hpp"

namespace ftgl
{

/**
 * @file   texture-atlas.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 *
 * @defgroup texture-atlas Texture atlas
 *
 * A texture atlas is used to pack several small regions into a single texture.
 *
 * The actual implementation is based on the article by Jukka Jylänki : "A
 * Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
 * Rectangle Bin Packing", February 27, 2010.
 * More precisely, this is an implementation of the Skyline Bottom-Left
 * algorithm based on C++ sources provided by Jukka Jylänki at:
 * http://clb.demon.fi/files/RectangleBinPack/
 *
 *
 * Example Usage:
 * @code
 * #include "texture-atlas.h"
 *
 * ...
 *
 * / Creates a new atlas of 512x512 with a depth of 1
 * texture_atlas_t * atlas = texture_atlas_new( 512, 512, 1 );
 *
 * // Allocates a region of 20x20
 * ivec4 region = texture_atlas_get_region( atlas, 20, 20 );
 *
 * // Fill region with some data
 * texture_atlas_set_region( atlas, region.x, region.y, region.width,
 * region.height, data, stride )
 *
 * ...
 *
 * @endcode
 *
 * @{
 */

/**
 * A texture atlas is used to pack several small regions into a single texture.
 */
class TextureAtlas
{
public:
    /**
     * Creates a new empty texture atlas.
     *
     * @param   width   width of the atlas
     * @param   height  height of the atlas
     * @param   depth   bit depth of the atlas
     * @return          a new empty texture atlas.
     *
     */
    TextureAtlas(size_t width, size_t height, size_t depth);

    /**
     *  Deletes a texture atlas.
     */
    ~TextureAtlas();


    /**
     *  Allocate a new region in the atlas.
     *
     *  @param width  width of the region to allocate
     *  @param height height of the region to allocate
     *  @return       Coordinates of the allocated region
     *
     */
    ivec4 get_region(size_t width, size_t height);

    /**
     *  Upload data to the specified atlas region.
     *
     *  @param x      x coordinate the region
     *  @param y      y coordinate the region
     *  @param width  width of the region
     *  @param height height of the region
     *  @param data   data to be uploaded into the specified region
     *  @param stride stride of the data
     *
     */
    void set_region(size_t x, size_t y, size_t width, size_t height,
            const unsigned char *data, size_t stride);

    int fit(size_t index, size_t width, size_t height);

    void merge();

    /**
     *  Remove all allocated regions from the atlas.
     */
    void clear();

    /**
     * Allocated nodes
     */
    std::vector<ivec3> nodes{};

    /**
     *  Width (in pixels) of the underlying texture
     */
    size_t width;

    /**
     * Height (in pixels) of the underlying texture
     */
    size_t height;

    /**
     * Depth (in bytes) of the underlying texture
     */
    const size_t depth;

    /**
     * Allocated surface size
     */
    size_t used{ 0 };

    /**
     * Texture identity (OpenGL)
     */
    unsigned int id{ 0 };

    /**
     * Atlas data
     */
    unsigned char *data;

    /**
     * Custom field
     */
    char dirty{ 1 };

};

/** @} */

}
