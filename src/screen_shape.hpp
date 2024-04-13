#ifndef SRC_SHAPE_HPP
#define SRC_SHAPE_HPP

#include <array>

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
    // Only triangle is supported
    struct Triangle
    {
        std::array<glm::vec<2, double>, 3> vertices_ = {};

        [[nodiscard]] inline bool test_point(const glm::vec<2, double>& point) const;
        inline glm::vec<2, double>& operator[](uint32_t idx);
        inline glm::vec<2, double> operator[](uint32_t idx) const;
    };

}; // namespace ras

inline bool ras::Triangle::test_point(const glm::vec<2, double>& point) const
{
    std::array<glm::vec<3, double>, 3> vertices = {glm::vec<3, double>(vertices_[1] - vertices_[0], 0),
                                                   glm::vec<3, double>(vertices_[2] - vertices_[1], 0),
                                                   glm::vec<3, double>(vertices_[0] - vertices_[2], 0)};
    std::array<glm::vec<3, double>, 3> points = {glm::vec<3, double>(point - vertices_[0], 0), //
                                                 glm::vec<3, double>(point - vertices_[1], 0), //
                                                 glm::vec<3, double>(point - vertices_[2], 0)};

    bool prev = glm::cross(vertices[0], points[0]).z > 0;
    if ((glm::cross(vertices[1], points[1]).z > 0) != prev)
    {
        return false;
    }
    return prev == glm::cross(vertices[2], points[2]).z > 0;
}

inline glm::vec<2, double>& ras::Triangle::operator[](uint32_t idx)
{
    return vertices_[idx];
}

inline glm::vec<2, double> ras::Triangle::operator[](uint32_t idx) const
{
    return vertices_[idx];
}

#endif // SRC_SHAPE_HPP
