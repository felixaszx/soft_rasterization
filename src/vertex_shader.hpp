#ifndef SRC_VERTEX_SHADER_HPP
#define SRC_VERTEX_SHADER_HPP

#include "shape.hpp"
#include "draw_call.hpp"
#include "parallel.hpp"

namespace ras
{
    using VertPosition = glm::vec4;

    using VertexAttributes = std::vector<std::byte*>;

    // type of attribute must be defined outside of the shader function
    std::vector<VertPosition> vertex_shader(const DrawCall& dc, //
                                            const std::function<void(VertPosition&, VertexAttributes&)>& shader);
}; // namespace ras

#endif // SRC_VERTEX_SHADER_HPP
