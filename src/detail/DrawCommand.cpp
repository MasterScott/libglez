/*
  Created on 03.08.18.
*/
#include <glez/detail/DrawCommand.hpp>

void glez::detail::drawcmd::DCUseProgram::execute()
{
    glUseProgram(shader);
}

void glez::detail::drawcmd::DCBindTexture::execute()
{
//    glBindTexture(GL_TEXTURE_2D, texture);
}

void glez::detail::drawcmd::DCDrawArrays::execute()
{
//    glDrawArrays(mode, start, count);
}

void glez::detail::drawcmd::DCCustom::execute()
{
    function(this + sizeof(DrawCmd) + sizeof(function));
}

void glez::detail::drawcmd::DCBufferRender::execute()
{
    buffer->render_range(mode, start, count);
}
