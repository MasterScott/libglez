/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vec234.hpp"
#include "vertex-buffer.hpp"

namespace ftgl
{

template<typename Vertex>
VertexBuffer<Vertex>::VertexBuffer(std::string fmt): format(std::move(fmt))
{
    size_t i, index = 0, stride = 0;
    const char *start = 0, *end = 0;
    GLchar *pointer = 0;

    start = format.c_str();
    do
    {
        char *desc = 0;
        end = (char *) (strchr(start + 1, ','));

        if (end == NULL)
        {
            desc = strdup(start);
        } else
        {
            desc = strndup(start, end - start);
        }

        auto attribute = VertexAttribute::parse(desc);
        if (!attribute)
            continue;

        start = end + 1;
        free(desc);
        attribute->pointer = pointer;

        GLuint attribute_size = 0;
        switch (attribute->type)
        {
        case GL_BOOL:
            attribute_size = sizeof(GLboolean);
            break;
        case GL_BYTE:
            attribute_size = sizeof(GLbyte);
            break;
        case GL_UNSIGNED_BYTE:
            attribute_size = sizeof(GLubyte);
            break;
        case GL_SHORT:
            attribute_size = sizeof(GLshort);
            break;
        case GL_UNSIGNED_SHORT:
            attribute_size = sizeof(GLushort);
            break;
        case GL_INT:
            attribute_size = sizeof(GLint);
            break;
        case GL_UNSIGNED_INT:
            attribute_size = sizeof(GLuint);
            break;
        case GL_FLOAT:
            attribute_size = sizeof(GLfloat);
            break;
        default:
            attribute_size = 0;
        }

        stride += attribute->size * attribute_size;
        pointer += attribute->size * attribute_size;

        attributes.push_back(*attribute);
        index++;
    } while (end && (index < MAX_VERTEX_ATTRIBUTE));

    for (auto& attr: attributes)
    {
        attr.stride = stride;
    }
}

template<typename Vertex>
VertexBuffer<Vertex>::~VertexBuffer()
{
#ifdef FREETYPE_GL_USE_VAO
    if (VAO_id)
    {
        glDeleteVertexArrays(1, &VAO_id);
    }
#endif
    if (vertices_id)
    {
        glDeleteBuffers(1, &vertices_id);
    }

    if (indices_id)
    {
        glDeleteBuffers(1, &indices_id);
    }
}

template<typename Vertex>
size_t VertexBuffer<Vertex>::size()
{
    return items.size();
}

static const std::string gl_types[9] = {
        "GL_BOOL", "GL_BYTE", "GL_UNSIGNED_BYTE",
        "GL_SHORT", "GL_UNSIGNED_SHORT", "GL_INT",
        "GL_UNSIGNED_INT", "GL_FLOAT", "GL_VOID"
};

template<typename Vertex>
void VertexBuffer<Vertex>::print()
{
    // FIXME vertices.size()
    fprintf(stderr, "%u vertices, %u indices\n", 0, indices.size());

    for (const auto& attr: attributes)
    {
        int j = 8;
        switch (attr.type)
        {
        case GL_BOOL:
            j = 0;
            break;
        case GL_BYTE:
            j = 1;
            break;
        case GL_UNSIGNED_BYTE:
            j = 2;
            break;
        case GL_SHORT:
            j = 3;
            break;
        case GL_UNSIGNED_SHORT:
            j = 4;
            break;
        case GL_INT:
            j = 5;
            break;
        case GL_UNSIGNED_INT:
            j = 6;
            break;
        case GL_FLOAT:
            j = 7;
            break;
        default:
            j = 8;
            break;
        }
        fprintf(stderr, "%s : %dx%s (+%p)\n", attr.name, attr.size, gl_types[j].c_str(), attr.pointer);
    }
}

template<typename Vertex>
void VertexBuffer<Vertex>::render_setup(GLenum mode)
{
#ifdef FREETYPE_GL_USE_VAO
    // Unbind so no existing VAO-state is overwritten,
    // (e.g. the GL_ELEMENT_ARRAY_BUFFER-binding).
    glBindVertexArray(0);
#endif

    if (state != CLEAN)
    {
        upload();
        state = CLEAN;
    }

#ifdef FREETYPE_GL_USE_VAO
    if (VAO_id == 0)
    {
        // Generate and set up VAO

        glGenVertexArrays(1, &VAO_id);
        glBindVertexArray(VAO_id);

        glBindBuffer(GL_ARRAY_BUFFER, vertices_id);

        for (auto& attr: attributes)
        {
            attr.enable();
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!indices.empty())
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
        }
    }

    // Bind VAO for drawing
    glBindVertexArray(VAO_id);
#else
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);

    for (auto& attr: attributes)
    {
        attr.enable();
    }

    if (!indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    }
#endif

    this->mode = mode;
}

template<typename Vertex>
void VertexBuffer<Vertex>::render_finish()
{
#ifdef FREETYPE_GL_USE_VAO
    glBindVertexArray(0);
#else
    for (auto& attr: attributes)
    {
        glDisableVertexAttribArray(attr.index);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
}

template<typename Vertex>
void VertexBuffer<Vertex>::render(GLenum mode)
{
    render_setup(mode);
    if (!indices.empty())
    {
        glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(mode, 0, vertices.size());
    }
    render_finish();
}

template<typename Vertex>
void VertexBuffer<Vertex>::render_item(size_t index)
{
    assert(index < items.size());

    auto item = items[index];

    if (!indices.empty())
    {
        glDrawElements(mode, item.icount, GL_UNSIGNED_INT,
                       (void *) (item.istart * sizeof(GLuint)));
    }
    else if (!vertices.empty())
    {
        glDrawArrays(mode, item.vstart * sizeof(Vertex), item.vcount);
    }
}

template<typename Vertex>
void VertexBuffer<Vertex>::upload()
{
    if (state == FROZEN)
        return;

    if (!vertices_id)
    {
        glGenBuffers(1, &vertices_id);
    }
    if (!indices_id)
    {
        glGenBuffers(1, &indices_id);
    }

    size_t vsize, isize;

    vsize = vertices.size() * sizeof(Vertex);
    isize = indices.size() * sizeof(typename decltype(indices)::value_type);

    // Always upload vertices first such that indices do not point to non
    // existing data (if we get interrupted in between for example).

    // Upload vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertices_id);
    if (vsize != GPU_vsize)
    {
        glBufferData(GL_ARRAY_BUFFER, vsize, vertices.data(),
                     GL_DYNAMIC_DRAW);
        GPU_vsize = vsize;
    }
    else
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, vertices.data());
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Upload indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    if (isize != GPU_isize)
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices.data(),
                     GL_DYNAMIC_DRAW);
        GPU_isize = isize;
    }
    else
    {
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, isize, indices.data());
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template<typename Vertex>
void VertexBuffer<Vertex>::clear()
{
    state = FROZEN;
    indices.clear();
    vertices.clear();
    items.clear();
    state = DIRTY;
}

template<typename Vertex>
void VertexBuffer<Vertex>::push_back_indices(const GLuint *indices, size_t icount)
{
    state |= DIRTY;
    this->indices.insert(this->indices.end(), indices, indices + icount);
}

template<typename Vertex>
void VertexBuffer<Vertex>::push_back_vertices(const Vertex *vertices, size_t vcount)
{
    state |= DIRTY;
    this->vertices.insert(this->vertices.end(), vertices, vertices + vcount);
}

template<typename Vertex>
void
VertexBuffer<Vertex>::insert_indices(size_t index, const GLuint *indices, size_t icount)
{
    assert(index < this->indices.size() + 1);

    state |= DIRTY;
    this->indices.insert(this->indices.begin() + index, indices, indices + icount);
}

template<typename Vertex>
void
VertexBuffer<Vertex>::insert_vertices(size_t index, const Vertex *vertices, size_t vcount)
{
    assert(index < this->vertices.size() + 1);

    state |= DIRTY;

    for (auto& i: indices)
    {
        if (i > index)
            i += vcount;
    }

    this->vertices.insert(this->vertices.begin() + index, vertices, vertices + vcount);
}

template<typename Vertex>
void VertexBuffer<Vertex>::erase_indices(size_t first, size_t last)
{
    assert(first < indices.size());
    assert(last <= indices.size());
    assert(last > first);

    state |= DIRTY;
    indices.erase(indices.begin() + first, indices.begin() + last);
}

template<typename Vertex>
void VertexBuffer<Vertex>::erase_vertices(size_t first, size_t last)
{
    assert(first < vertices->size);
    assert(last <= vertices->size);
    assert(last > first);

    state |= DIRTY;
    for (auto& i: indices)
    {
        if (i > first)
            i -= (last - first);
    }
    vertices.erase(vertices.begin() + first, vertices.begin() + last);
}

template<typename Vertex>
size_t VertexBuffer<Vertex>::push_back(const Vertex *vertices, size_t vcount,
                               const GLuint *indices, size_t icount)
{
    return insert(items.size(), vertices, vcount, indices, icount);
}

template<typename Vertex>
size_t VertexBuffer<Vertex>::insert(size_t index, const Vertex *vertices, size_t vcount,
                            const GLuint *indices, size_t icount)
{
    state = FROZEN;

    // Push back vertices
    auto vstart = this->vertices.size();
    push_back_vertices(vertices, vcount);

    // Push back indices
    auto istart = this->indices.size();
    push_back_indices(indices, icount);

    // Update indices within the vertex buffer
    for (auto i = istart; i < this->indices.size(); ++i)
    {
        this->indices[i] += vstart;
    }

    // Insert item
    items.emplace(items.begin() + index, ivec4 { vstart, vcount, istart, icount });

    state = DIRTY;
    return index;
}

template<typename Vertex>
void VertexBuffer<Vertex>::erase(size_t index)
{
    assert(index < items.size());

    auto item = items[index];
    auto vstart = (size_t)item.vstart;
    auto vcount = (size_t)item.vcount;
    auto istart = (size_t)item.istart;
    auto icount = (size_t)item.icount;

    // Update items
    for (auto& it: items)
    {
        if (it.vstart > vstart)
        {
            it.vstart -= vcount;
            it.istart -= icount;
        }
    }

    state = FROZEN;
    erase_indices(istart, istart + icount);
    erase_vertices(vstart, vstart + vcount);
    items.erase(items.begin() + index);
    state = DIRTY;
}

template<typename Vertex>
void VertexBuffer<Vertex>::render_items(size_t start, size_t count)
{
    render_setup(mode);
    if (!indices.empty())
    {
        glDrawElements(mode, count, GL_UNSIGNED_INT, (const void *)(start * sizeof(GLuint)));
    }
    else
    {
        glDrawArrays(mode, start, count);
    }
    render_finish();
}

}