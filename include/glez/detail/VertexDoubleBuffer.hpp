/*
  Created on 03.08.18.
*/

#pragma once

#include <memory>

#include <glez/detail/freetype-gl/vertex-buffer.hpp>
#include <glez/DrawQueue.hpp>

namespace glez::detail
{

template<typename Vertex>
class VertexDoubleBuffer
{
public:
    explicit VertexDoubleBuffer(std::string format);

    ftgl::VertexBuffer<Vertex> *operator*();

    std::unique_ptr<ftgl::VertexBuffer<Vertex>> buffers[2];
};

template<typename Vertex>
VertexDoubleBuffer<Vertex>::VertexDoubleBuffer(std::string format)
{
    for (auto &buffer : buffers)
        buffer = std::make_unique<ftgl::VertexBuffer<Vertex>>(format);
}

template<typename Vertex>
ftgl::VertexBuffer<Vertex> *VertexDoubleBuffer<Vertex>::operator*()
{
    return buffers[queue::getBufferIndex()].get();
}

}