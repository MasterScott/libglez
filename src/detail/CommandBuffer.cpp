/*
  Created on 03.08.18.
*/
#include <glez/detail/CommandBuffer.hpp>

#include <cstring>

void glez::detail::CommandBuffer::pushCommand(drawcmd::DrawCmd &cmd)
{
    reserve(cmd.size);
    std::memcpy((void *)((size_t)data + offset), &cmd, cmd.size);
    offset += cmd.size;
}

void glez::detail::CommandBuffer::reset()
{
    offset = 0;
}

glez::detail::CommandBuffer::CommandBuffer()
{
    data = std::malloc(capacity);
}

glez::detail::CommandBuffer::~CommandBuffer()
{
    std::free(data);
}

void glez::detail::CommandBuffer::reserve(size_t size)
{
    bool changed{ false };
    while (offset + size > capacity)
    {
        capacity *= 2;
        changed = true;
    }
    if (changed)
        data = std::realloc(data, capacity);
}

