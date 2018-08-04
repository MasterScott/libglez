/*
  Created on 03.08.18.
*/

#pragma once

#include <cstddef>
#include "DrawCommand.hpp"

namespace glez::detail
{

class CommandBuffer
{
public:
    CommandBuffer();

    CommandBuffer(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&&) = delete;

    ~CommandBuffer();

    void render();

    void begin();

    void end();

    void pushCommand(drawcmd::DrawCmd& cmd);

protected:
    void reserve(size_t size);

    std::vector<ftgl::IVertexBuffer *> delayed_upload_buffers{};

    void *data{ nullptr };
    size_t capacity{ 256 };
    size_t offset{ 0 };
};

}