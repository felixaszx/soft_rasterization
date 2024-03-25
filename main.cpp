#include <iostream>
#include "screen.hpp"
#include "vertex_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    Screen screen(1920, 1080);

    while (screen.run())
    {
    }
    return EXIT_SUCCESS;
}