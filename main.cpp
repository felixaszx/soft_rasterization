#include <iostream>
#include "screen.hpp"
#include "vertex_shader.hpp"
#include "fragment_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    const uint64_t WIDTH = 1920;
    const uint64_t HEIGHT = 1080;
    Screen screen(WIDTH, HEIGHT);
    *screen[{10, 20}] = 255;


    while (screen.run())
    {
    }
    return EXIT_SUCCESS;
}