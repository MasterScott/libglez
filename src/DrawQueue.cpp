/*
  Created on 04.08.18.
*/
#include <glez/DrawQueue.hpp>
#include <glez/detail/CommandBuffer.hpp>
#include <array>
#include <glez/Render.hpp>
#include <glez/draw.hpp>

static size_t buffer_index{ 0 };

static struct State
{
    ftgl::IVertexBuffer *buffer{ nullptr };
    GLenum mode{ GL_TRIANGLES };
    GLuint start{ 0 };
    GLuint count{ 0 };
} state;

static size_t getReadBufferIndex()
{
    return 1 - buffer_index;
}

static std::array<glez::detail::CommandBuffer, 2> buffers{};

static void swapBuffers()
{
    // TODO lock

    buffer_index = 1 - buffer_index;
    // swap command buffer

    // TODO unlock
}

void glez::queue::begin()
{
    // lock buffer
    // clear render buffer
    draw::flushVertexBuffers();
    buffers[buffer_index].begin();
}

void glez::queue::end()
{
    buffers[buffer_index].end();
    // unlock buffer
}

void glez::queue::render()
{
    swapBuffers();

    glez::render::glStateSetup();
    buffers[getReadBufferIndex()].render();
    glez::render::glStateRestore();
}

size_t glez::queue::getBufferIndex()
{
    return buffer_index;
}

void glez::queue::commit()
{
    if (state.count == 0)
        return;

    detail::drawcmd::DCBufferRender br{ state.buffer };
    br.count = state.count;
    br.start = state.start;
    buffers[getBufferIndex()].pushCommand(br);
    state = State{};
}

void glez::queue::bindBuffer(ftgl::IVertexBuffer *buffer, GLenum mode)
{
    state.buffer = buffer;
    state.mode = mode;
}

void glez::queue::pushIndices(size_t start, size_t count)
{
    if (start != state.start + state.count)
    {
        commit();
        state.start = start;
        state.count = count;
    }
    else
    {
        state.count += count;
    }
}

void glez::queue::bindTexture(GLuint texture)
{
    detail::drawcmd::DCBindTexture cmd{};
    cmd.texture = texture;
    buffers[getBufferIndex()].pushCommand(cmd);
}

void glez::queue::useProgram(GLuint program)
{
    detail::drawcmd::DCUseProgram cmd{};
    cmd.shader = program;
    buffers[getBufferIndex()].pushCommand(cmd);
}

