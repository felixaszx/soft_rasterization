#include <iostream>
#include "screen.hpp"
#include "vertex_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    std::atomic_size_t test = 0;
    Screen screen(1280, 720);

    while (screen.run())
    {
    }
    return EXIT_SUCCESS;
}