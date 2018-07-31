/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#pragma once

#include "opengl.hpp"
#include "vertex-attribute.hpp"
#include "vec234.hpp"

#include <string>
#include <vector>

namespace ftgl
{

/**
 * @file   vertex-buffer.h
 * @author Nicolas Rougier (Nicolas.Rougier@inria.fr)
 * @date   April, 2012
 *
 * @defgroup vertex-buffer Vertex buffer
 *
 * @{
 */

/**
 * Generic vertex buffer.
 */
 template<typename Vertex>
class VertexBuffer
{
public:
    using vertex_type = Vertex;

    static_assert(std::is_pod<Vertex>::value);

    /**
     * Buffer status
     */
    static constexpr char CLEAN = 0;
    static constexpr char DIRTY = 1;
    static constexpr char FROZEN = 2;

    /**
     * Creates an empty vertex buffer.
     *
     * @param  format a string describing vertex format.
     */
    explicit VertexBuffer(std::string fmt);

    /**
     * Deletes vertex buffer and releases GPU memory.
     */
     ~VertexBuffer();

    /**
     *  Returns the number of items in the vertex buffer
     *
     *  @param  self  a vertex buffer
     *  @return       number of items
     */
    size_t size();

    /**
     * Print information about a vertex buffer
     */
    void print();

    /**
     * Prepare vertex buffer for render.
     *
     * @param  mode  render mode
     */
    void render_setup(GLenum mode);

    /**
     * Finish rendering by setting back modified states
     *
     * @param  self  a vertex buffer
     */
    void render_finish();

    /**
     * Render vertex buffer.
     *
     * @param  mode  render mode
     */
    void render(GLenum mode);

    /**
     * Render a specified item from the vertex buffer.
     *
     * @param  index index of the item to be rendered
     */
    void render_item(size_t index);

    /**
     * Upload buffer to GPU memory.
     */
    void upload();

    /**
     * Clear all items.
     */
    void clear();

    /**
     * Appends indices at the end of the buffer.
     *
     * @param  indices  indices to be appended
     * @param  icount   number of indices to be appended
     *
     * @private
     */
    void push_back_indices(const GLuint *indices, size_t icount);

    /**
     * Appends vertices at the end of the buffer.
     *
     * @note Internal use
     *
     * @param  vertices vertices to be appended
     * @param  vcount   number of vertices to be appended
     *
     * @private
     */
    void push_back_vertices(const Vertex *vertices, size_t vcount);

    /**
     * Insert indices in the buffer.
     *
     * @param  index   location before which to insert indices
     * @param  indices indices to be appended
     * @param  icount  number of indices to be appended
     *
     * @private
     */
    void insert_indices(size_t index, const GLuint *indices, size_t icount);

    /**
     * Insert vertices in the buffer.
     *
     * @param  index    location before which to insert vertices
     * @param  vertices vertices to be appended
     * @param  vcount   number of vertices to be appended
     *
     * @private
     */
    void insert_vertices(size_t index, const Vertex *vertices, size_t vcount);

    /**
     * Erase indices in the buffer.

     * @param  first  the index of the first index to be erased
     * @param  last   the index of the last index to be erased
     *
     * @private
     */
    void erase_indices(size_t first, size_t last);

    /**
     * Erase vertices in the buffer.
     *
     * @param  first  the index of the first vertex to be erased
     * @param  last   the index of the last vertex to be erased
     *
     * @private
     */
    void erase_vertices(size_t first, size_t last);

    /**
     * Append a new item to the collection.
     *
     * @param  vcount   number of vertices
     * @param  vertices raw vertices data
     * @param  icount   number of indices
     * @param  indices  raw indices data
     */
    size_t push_back(const Vertex *vertices, size_t vcount, const GLuint *indices,
                                   size_t icount);

    /**
     * Insert a new item into the vertex buffer.
     *
     * @param  index     location before which to insert item
     * @param  vertices  raw vertices data
     * @param  vcount    number of vertices
     * @param  indices   raw indices data
     * @param  icount    number of indices
     */
    size_t insert(size_t index, const Vertex *vertices, size_t vcount,
                                const GLuint *indices, size_t icount);

    /**
     * Erase an item from the vertex buffer.
     *
     * @param  index    index of the item to be deleted
     */
    void erase(size_t index);

    /** Format of the vertex buffer. */
    const std::string format;

    /** Vector of vertices. */
    std::vector<Vertex> vertices{};

#ifdef FREETYPE_GL_USE_VAO
    /** GL identity of the Vertex Array Object */
    GLuint VAO_id;
#endif

    /** GL identity of the vertices buffer. */
    GLuint vertices_id{ 0 };

    /** Vector of indices. */
    std::vector<GLuint> indices{};

    /** GL identity of the indices buffer. */
    GLuint indices_id{ 0 };

    /** Current size of the vertices buffer in GPU */
    size_t GPU_vsize{ 0 };

    /** Current size of the indices buffer in GPU*/
    size_t GPU_isize{ 0 };

    /** GL primitives to render. */
    GLenum mode{ GL_TRIANGLES };

    /** Whether the vertex buffer needs to be uploaded to GPU memory. */
    char state{ DIRTY };

    /** Individual items */
    std::vector<ivec4> items{};

    /** Array of attributes. */
    std::vector<VertexAttribute> attributes{};
};

/** @} */

}

#include "vertex-buffer.impl.hpp"