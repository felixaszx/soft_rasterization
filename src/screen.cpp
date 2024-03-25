#include "screen.hpp"

namespace ras
{
    Screen::Screen(uint32_t width, uint32_t height)
        : width_(width),
          height_(height),
          back_buffer_(width, height)
    {
        image_ = std::make_unique<Fl_RGB_Image>(back_buffer_, width, height, 4);
        image_->alloc_array = 0;
        image_->array = back_buffer_;

        image_box_ = std::make_unique<Fl_Box>(0, 0, width, height);
        image_box_->image(*image_);

        window_ = std::make_unique<Fl_Window>(width, height);
        window_->add(*image_box_);
        window_->end();
        window_->show();
    }

    void Screen::test_triangle(const Triangle& tri, const std::function<void(PixelCord, ScreenBuffer&)>& func)
    {
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

        for (uint32_t i = bounding_box[0].x; i < bounding_box[1].x; i++)
        {
            for (uint32_t k = bounding_box[0].y; k < bounding_box[1].y; k++)
            {
                if (tri.test_point({i, k}))
                {
                    func({i, k}, back_buffer_);
                }
            }
        }
    }

    bool Screen::run()
    {
        image_->uncache();
        image_box_->redraw();
        return Fl::check();
    }

    ScreenBuffer::ScreenBuffer(uint32_t width, uint32_t height)
        : data_(width * height * 4, 0),
          width_(width)
    {
        for (int i = 3; i < width * height * 4; i += 4)
        {
            data_[i] = 255;
        }
    }

}; // namespace ras