/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <glez/detail/freetype-gl/texture-atlas.hpp>
#include <GL/glew.h>

#include "glez/detail/freetype-gl/texture-atlas.hpp"

namespace ftgl
{

int TextureAtlas::fit(size_t index, size_t width, size_t height)
{
    auto node = nodes[index];
    auto x = node.x;
    auto y = node.y;
    auto width_left = width;
    auto i = index;

    if ((x + width) > (this->width - 1))
    {
        return -1;
    }

    while (width_left > 0)
    {
        node = nodes[i];
        if (node.y > y)
        {
            y = node.y;
        }
        if ((y + height) > (this->height - 1))
        {
            return -1;
        }
        width_left -= node.z;
        ++i;
    }
    return y;
}

void TextureAtlas::merge()
{

    for (auto it = nodes.begin(); it != nodes.end() - 1;)
    {
        auto& node = *it;
        auto next = *(it + 1);
        if (node.y == next.y)
        {
            node.z += next.z;
            it = nodes.erase(it);
        }
        else
            ++it;
    }
}

TextureAtlas::TextureAtlas(size_t width, size_t height, size_t depth): width(width), height(height), depth(depth)
{
    assert((depth == 1) || (depth == 3) || (depth == 4));

    // We want a one pixel border around the whole atlas to avoid any artefact
    // when sampling texture

    data = new unsigned char[width * height * depth];

    clear();

    if (data == nullptr)
    {
        fprintf(stderr, "line %d: No more memory for allocating data\n",
                __LINE__);
        exit(EXIT_FAILURE);
    }
}

ivec4 TextureAtlas::get_region(size_t width, size_t height)
{
    int best_index{ -1 };
    size_t best_height{ UINT_MAX };
    size_t best_width{ UINT_MAX };
    ivec4 region = {{0, 0, (int)width, (int)height}};

    for (auto i = 0; i < nodes.size(); ++i)
    {
        auto y = fit((size_t)i, width, height);

        if (y < 0)
            continue;

        auto& node = nodes[i];
        if (((y + height) < best_height) ||
            (((y + height) == best_height) &&
             (node.z > 0 && (size_t) node.z < best_width)))
        {
            best_height = y + height;
            best_index = i;
            best_width = (size_t)node.z;
            region.x = node.x;
            region.y = y;
        }
    }

    if (best_index == -1)
    {
        region.x = -1;
        region.y = -1;
        region.width = 0;
        region.height = 0;
        return region;
    }

    {
        ivec3 node{ region.x, (int)(region.y + height), (int)width };
        nodes.insert(nodes.begin() + best_index, node);
    }

    for (auto it = nodes.begin() + best_index + 1; it != nodes.end();)
    {
        auto& node = *it;
        auto prev = *(it - 1);

        if (node.x < (prev.x + prev.z))
        {
            int shrink = prev.x + prev.z - node.x;
            node.x += shrink;
            node.z -= shrink;
            if (node.z <= 0)
                it = nodes.erase(it);
            else
                break;
        }
        else
            break;
    }

    merge();
    used += width * height;
    return region;
}

void TextureAtlas::set_region(size_t x, size_t y, size_t width, size_t height,
                              const unsigned char *data, size_t stride)
{
    assert(x);
    assert(y);
    assert(x < (this->width - 1));
    assert((x + width) <= (this->width - 1));
    assert(y < (this->height - 1));
    assert((y + height) <= (this->height - 1));

    size_t i{ 0 };

    for (i = 0; i < height; ++i)
    {
        memcpy(&this->data[((y + i) * width + x) * depth],
               &data[(i * stride)], width * depth);
    }

    dirty = true;
}

void TextureAtlas::clear()
{
    nodes.clear();
    used = 0;

    // We want a one pixel border around the whole atlas to avoid any artifact
    // when sampling texture
    nodes.push_back({ 1, 1, (int)width - 2 });

    memset(data, 0, width * height * depth);
}

TextureAtlas::~TextureAtlas()
{
    delete[] data;
}

void TextureAtlas::upload()
{
    if (id == 0)
        glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    dirty = false;
}

}