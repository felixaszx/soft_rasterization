#ifndef SRC_DRAW_CALL_HPP
#define SRC_DRAW_CALL_HPP

#include <vector>
#include <memory>
#include <functional>
#include <thread>

namespace ras
{
    struct DrawCall
    {
        std::vector<size_t> attrib_strides_ = {};
        std::vector<std::vector<std::byte>*> attributes_ = {};
        std::vector<size_t>* indices_ = nullptr;
        size_t instance_ = 1;
    };
}; // namespace ras

#endif // SRC_DRAW_CALL_HPP
