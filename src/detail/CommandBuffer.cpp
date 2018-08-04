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

void glez::detail::CommandBuffer::render()
{
    size_t read_index{ 0 };
    while (read_index < offset)
    {
        auto c = reinterpret_cast<drawcmd::DrawCmd *>((uintptr_t)data + read_index);
        switch (c->type)
        {
        case drawcmd::DrawCmd::END:
        {
            return;
        }
        case drawcmd::DrawCmd::USE_PROGRAM:
        {
            auto cmd = reinterpret_cast<drawcmd::DCUseProgram *>((uintptr_t)data + read_index);
            cmd->execute();
            break;
        }
        case drawcmd::DrawCmd::BIND_TEXTURE:
        {
            auto cmd = reinterpret_cast<drawcmd::DCBindTexture *>((uintptr_t)data + read_index);
            cmd->execute();
            break;
        }
        case drawcmd::DrawCmd::BUFFER_RENDER:
        {
            auto cmd = reinterpret_cast<drawcmd::DCBufferRender *>((uintptr_t)data + read_index);
            cmd->execute();
            break;
        }
        case drawcmd::DrawCmd::CUSTOM:
        {
            auto cmd = reinterpret_cast<drawcmd::DCCustom *>((uintptr_t)data + read_index);
            cmd->execute();
            break;
        }
        }
        read_index += c->size;
    }
}

void glez::detail::CommandBuffer::end()
{
    drawcmd::DrawCmd end_command{};
    pushCommand(end_command);
}

void glez::detail::CommandBuffer::begin()
{
    offset = 0;
}

