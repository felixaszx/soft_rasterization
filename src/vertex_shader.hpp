#ifndef SRC_VERTEX_SHADER_HPP
#define SRC_VERTEX_SHADER_HPP

#include <array>
#include <vector>
#include <memory>
#include <tuple>
#include <functional>
#include <thread>
#include <future>
#include <concepts>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_INLINE
#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace ras
{
    using ClipPositon = glm::vec<3, double>;

    template <typename ReturnType = ClipPositon, typename IndexType = uint64_t, typename... AttributeTypes>
    std::vector<ReturnType> excute_vertex_shader(const std::function<ReturnType(AttributeTypes... arg)>& shader,
                                                 const std::vector<IndexType>& indices, //
                                                 size_t vertex_count,
                                                 const typename std::vector<AttributeTypes>::iterator&... attributes)
    {
        std::vector<ReturnType> returns;
        std::vector<IndexType> indices_copy = indices;
        if (indices_copy.empty())
        {
            indices_copy.resize(vertex_count);
            for (size_t i = 0; i < vertex_count; i++)
            {
                indices_copy[i] = i;
            }
        }

        returns.resize(indices_copy.size());
        size_t group_size = indices_copy.size() / std::thread::hardware_concurrency();
        size_t remain_size = indices_copy.size() % std::thread::hardware_concurrency();
        std::vector<std::future<void>> ths;
        ths.reserve(std::thread::hardware_concurrency());

        for (size_t t = 0; t < std::thread::hardware_concurrency(); t++)
        {
            ths.emplace_back(std::async(
                [&, t]()
                {
                    for (size_t i = t * group_size; i < (t + 1) * group_size; i++)
                    {
                        returns[i] = shader(attributes[i]...);
                    }
                }));
        }

        for (size_t i = group_size * std::thread::hardware_concurrency(); i < indices_copy.size(); i++)
        {
            returns[i] = shader(attributes[i]...);
        }

        for (auto& f : ths)
        {
            f.wait();
        }

        return returns;
    }

}; // namespace ras

#endif // SRC_VERTEX_SHADER_HPP
