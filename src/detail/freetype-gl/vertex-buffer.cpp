/*
  Created on 03.08.18.
*/

#include <glez/detail/freetype-gl/vertex-buffer.hpp>

void ftgl::IVertexBuffer::render_setup(GLenum mode)
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

void ftgl::IVertexBuffer::render_finish()
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
