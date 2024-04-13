#ifndef SRC_FRAGMENT_SHADER_HPP
#define SRC_FRAGMENT_SHADER_HPP

#include "vertex_shader.hpp"
#include "screen_shape.hpp"

namespace ras
{
    template <typename T>
    concept AcceptableFragmentInput = requires(T input) //
    {
        requires std::convertible_to<decltype(input.x), double>;
        requires std::convertible_to<decltype(input.y), double>;
        requires std::convertible_to<decltype(input.z), double>;
    };

    template <typename T>
    concept AcceptableAttachment = requires(T attachment) //
    { requires std::same_as<decltype(attachment[0][0]), typename T::Pixel&>; };

    // depth test need to be done by hand.

    template <typename InputType, typename... AttachmentType>
        requires AcceptableFragmentInput<InputType> && (AcceptableFragmentInput<AttachmentType> && ...)
    void excute_fragment_shader(
        size_t width, size_t height, const std::vector<InputType>& inputs,
        std::function<void(InputType, uint64_t x, uint64_t y, typename AttachmentType::Pixel&...)> fragment_shader,
        AttachmentType&... atchms)
    {
        size_t counter = 0;
        Triangle tri;
        for (auto& input : inputs)
        {
            if (counter < 3)
            {
                tri.vertices_[counter].x = ((input.x + 1) / double(2)) * width;
                tri.vertices_[counter].y = ((input.y + 1) / double(2)) * height;
                counter++;
            }
            else
            {
                // rasterization begins here
                counter = 0;
                std::array<glm::vec2, 2> bounding_box = {};

                for (int i = 0; i < 3; i++)
                {
                    if (tri[i].x <= bounding_box[0].x)
                    {
                        bounding_box[0].x = tri[i].x;
                    }
                    else
                    {
                        bounding_box[1].x = tri[i].x;
                    }

                    if (tri[i].y >= bounding_box[1].y)
                    {
                        bounding_box[1].y = tri[i].y;
                    }
                    else
                    {
                        bounding_box[0].y = tri[i].y;
                    }
                }

                for (size_t i = bounding_box[0].x; i < bounding_box[1].x; i++)
                {
                    for (size_t k = bounding_box[0].y; k < bounding_box[1].y; k++)
                    {
                        if (tri.test_point({i, k}))
                        {
                            // calculate interporlation here  
                        }
                    }
                }
            }
        }
    }

}; // namespace ras

#endif // SRC_FRAGMENT_SHADER_HPP
