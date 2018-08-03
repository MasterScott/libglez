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



    void pushCommand(drawcmd::DrawCmd& cmd);

    void reset();

protected:
    void reserve(size_t size);

    void *data{ nullptr };
    size_t capacity{ 256 };
    size_t offset{ 0 };
};

}