#include <iostream>
#include "screen.hpp"
#include "vertex_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    Screen screen(1280, 720);
    std::vector<std::function<void()>> funcs(1000);
    for (int i = 0; i < 1000; i++)
    {
        funcs.emplace_back(
            []()
            {
                for (int i = 0; i < 10; i++)
                {
                }
            });
    }
    AdvanceThreadPool pool(20);
    pool.add_tasks(funcs, 5);
    pool.prepare();
    while (screen.run())
    {
    }
    return EXIT_SUCCESS;
}