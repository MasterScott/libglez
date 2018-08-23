/*
  Created on 03.08.18.
*/

#include <glez/Render.hpp>

static struct RenderState {
    GLuint texture{ 0 };
    GLenum mode{ GL_TRIANGLES };
    GLuint program{ 0 };
    ftgl::IVertexBuffer *buffer{ nullptr };
    ftgl::TextureAtlas *atlas{ nullptr };
} render_state{};

void glez::render::bindTexture(GLuint texture)
{
    if (render_state.texture != texture)
    {
        commit();
        glBindTexture(GL_TEXTURE_2D, texture);
        render_state.texture = texture;
        render_state.atlas = nullptr;
    }
}

void glez::render::useProgram(GLuint program)
{
    if (render_state.program != program)
    {
        commit();
        glUseProgram(program);
        render_state.program = program;
    }
}

void glez::render::bindVertexBuffer(ftgl::IVertexBuffer *buffer, GLenum mode)
{
    if (render_state.buffer != buffer || render_state.mode != mode)
    {
        commit();
        buffer->clear();
        render_state.mode = mode;
        render_state.buffer = buffer;
    }
}

void glez::render::glStateSetup()
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT |
                 GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_FOG_COORD_ARRAY);
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
}

void glez::render::glStateRestore()
{
    glPopClientAttrib();
    glPopAttrib();
}

void glez::render::bindTextureAtlas(ftgl::TextureAtlas &atlas)
{
    if (render_state.atlas != &atlas)
    {
        commit();
        if (atlas.dirty)
            atlas.upload();
        glBindTexture(GL_TEXTURE_2D, atlas.id);
        render_state.atlas = &atlas;
        render_state.texture = 0;
    }
}

void glez::render::commit()
{
    if (render_state.buffer)
    {
        render_state.buffer->render(render_state.mode);
        reset();
    }
}

void glez::render::begin()
{
     // TODO
     glStateSetup();
     // Reset the render state by using a default constructed object
     render_state = RenderState{};
}

void glez::render::end()
{
    commit();
    glStateRestore();
}

void glez::render::reset()
{
    if (render_state.buffer)
    {
        render_state.buffer->clear();
    }
}

