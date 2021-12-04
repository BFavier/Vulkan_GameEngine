#include <iostream>
#include <GameEngine/game_engine.hpp>
#include <vector>
#include <iostream>
using namespace GameEngine;

int main()
{
    Engine::initialize({"VK_LAYER_KHRONOS_validation"});
    GPU gpu = GPU::get_best_device();
    WindowSettings settings;
    settings.transparent = true;
    Window window(gpu, settings);
    Mouse& mouse = window.mouse;
    while(!window.closing())
    {
        double dx = mouse.dx();
        if (dx != 0)
        {
            std::cout << dx << std::endl;
        }
        window.update();
    }
    return EXIT_SUCCESS;
}
