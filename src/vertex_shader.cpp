#include "vertex_shader.hpp"

namespace ras
{

    std::vector<VertPosition> vertex_shader(const DrawCall& dc, //
                                            const std::function<void(VertPosition&, VertexAttributes&)>& shader)
    {
        std::vector<VertPosition> pos;
        VertexAttributes attributes = {};
        attributes.resize(dc.attributes_.size());

        if (dc.indices_->size() != 0)
        {
            pos.reserve(dc.indices_->size());
            for (auto i : *dc.indices_)
            {
                for (int a = 0; a < dc.attributes_.size(); a++)
                {
                    attributes[a] = dc.attributes_[a]->data() + i * dc.attrib_strides_[a];
                }
                shader(pos.emplace_back(), attributes);
            }
        }
        else
        {
            pos.reserve(dc.attributes_[0]->size());
            for (int v = 0; v < dc.attributes_[0]->size(); v++)
            {
                for (int a = 0; a < dc.attributes_.size(); a++)
                {
                    attributes[a] = dc.attributes_[a]->data() + v * dc.attrib_strides_[a];
                }
                shader(pos.emplace_back(), attributes);
            }
        }

        return pos;
    }
}; // namespace ras