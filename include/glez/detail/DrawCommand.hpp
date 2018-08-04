/*
  Created on 31.07.18.
*/

#pragma once

#include <GL/glew.h>
#include <functional>
#include "freetype-gl/vertex-buffer.hpp"

namespace glez::detail::drawcmd
{

struct DrawCmd
{
    enum type_enum: int
    {
        END,
        USE_PROGRAM,
        BIND_TEXTURE,
        BUFFER_RENDER,
        CUSTOM
    };

    DrawCmd()
    {
        size = sizeof(DrawCmd);
    }

    type_enum type{ END };
    size_t size{ 0 };
};

struct DCUseProgram: public DrawCmd
{
    DCUseProgram()
    {
        type = USE_PROGRAM;
        size = sizeof(DCUseProgram);
    }

    void execute();

    GLuint shader{ 0 };
};

struct DCBindTexture: public DrawCmd
{
    DCBindTexture()
    {
        type = BIND_TEXTURE;
        size = sizeof(DCBindTexture);
    }

    void execute();

    GLuint texture{ 0 };
};

struct DCDrawArrays: public DrawCmd
{
    DCDrawArrays()
    {
        size = sizeof(DCDrawArrays);
    }

    void execute();

    GLenum mode{ GL_TRIANGLES };
    GLuint start{ 0 };
    GLuint count{ 0 };
};

struct DCBufferRender: public DrawCmd
{
    explicit DCBufferRender(ftgl::IVertexBuffer *buffer): buffer(buffer)
    {
        type = BUFFER_RENDER;
        size = sizeof(DCBufferRender);
    }

    void execute();

    ftgl::IVertexBuffer *buffer{ nullptr };
    GLenum mode{ GL_TRIANGLES };
    GLuint start{ 0 };
    GLuint count{ 0 };
};

struct DCCustom: public DrawCmd
{
    using function_type = void(*)(void *);

    explicit DCCustom(function_type fn): function(fn)
    {
        type = CUSTOM;
        size = sizeof(DCCustom);
    }

    void execute();

    function_type function;
};

}