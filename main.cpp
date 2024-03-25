#include <iostream>
#include "screen.hpp"
#include "vertex_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    Screen screen(1280, 720);

    RingQueue<int> rq(100);

    for (int i = 0; i < 100; i++)
    {
        rq.push(i);
        if (i % 2)
        {
            int a = 0;
            rq.pop(a);
        }
    }
    while (screen.run())
    {
    }
    return EXIT_SUCCESS;
}