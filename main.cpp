#include <iostream>
#include "vertex_shader.hpp"

int main(int argc, char** argv)
{
    using namespace ras;
    std::vector<float> f_test(10000000);
    std::vector<int> i_test(10000000);
    std::atomic_size_t counter = 0;
    for (int i = 0; i < 10000000; i++)
    {
        f_test[i] = i + 1;
        i_test[i] = i;
    }

    std::function<ClipPositon(float, int)> f = [&](float a, int b) { return ClipPositon{}; };
    auto k = excute_vertex_shader<ClipPositon, uint16_t, float, int>(f, {}, 10000000, f_test.begin(), i_test.begin());
    return EXIT_SUCCESS;
}