/*
  Created on 03.08.18.
*/

#pragma once

#include <memory>

namespace ftgl
{
template<typename Vertex>
class VertexBuffer;
}

namespace glez::detail
{

template<typename Vertex>
class VertexDoubleBuffer
{
public:
    explicit VertexDoubleBuffer(std::string format);

    std::unique_ptr<ftgl::VertexBuffer<Vertex>> buffers[2];
};

}