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
        USE_PROGRAM,
        BIND_TEXTURE,
        DRAW_ARRAYS,
        CUSTOM
    };

    int type{ 0 };
    size_t size{ 0 };
};

struct DCUseProgram: public DrawCmd
{
    void execute();

    GLuint shader{ 0 };
};

struct DCBindTexture: public DrawCmd
{
    void execute();

    GLuint texture{ 0 };
};

struct DCDrawArrays: public DrawCmd
{
    void execute();

    GLenum mode{ GL_TRIANGLES };
    GLuint start{ 0 };
    GLuint count{ 0 };
};

struct DCBufferRender: public DrawCmd
{
    void execute();

    ftgl::IVertexBuffer *buffer;
};

struct DCCustom: public DrawCmd
{
    using function_type = void(*)(void *);

    void execute();

    function_type function;
};

}