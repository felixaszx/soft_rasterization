#ifndef SRC_SCREEN_HPP
#define SRC_SCREEN_HPP

#include <memory>
#include <functional>
#include <thread>

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_RGB_Image.H>

#include "screen_shape.hpp"

namespace ras
{
    using PixelCord = glm::vec<2, uint32_t>;
    class ScreenBuffer
    {
      private:
        uint32_t width_ = 0;
        std::vector<uint8_t> data_ = {};

      public:
        ScreenBuffer(uint32_t width, uint32_t height);

        operator uchar*() { return data_.data(); }
        uint8_t* operator[](PixelCord idx) { return data_.data() + 4 * (width_ * idx.y + idx.x); }
    };

    class Screen
    {
      private:
        std::unique_ptr<Fl_Window> window_ = nullptr;
        std::unique_ptr<Fl_RGB_Image> image_ = nullptr;
        std::unique_ptr<Fl_Box> image_box_ = nullptr;
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        ScreenBuffer back_buffer_;

      public:
        Screen(uint32_t width, uint32_t height);

        uint8_t* operator[](PixelCord idx) { return back_buffer_[idx]; }
        void test_triangle(const Triangle& tri, const std::function<void(PixelCord, ScreenBuffer&)>& func);
        bool run();
    };
}; // namespace ras

#endif // SRC_SCREEN_HPP
