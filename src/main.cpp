#include <iostream>
#include <GameEngine/game_engine.hpp>
#include <vector>
#include <iostream>
using namespace GameEngine;

int main()
{
    Engine::initialize({"VK_LAYER_KHRONOS_validation"});
    Window window;
    Mouse& mouse = window.mouse;
    GPU gpu = GPU::get_best_device();
    while(!window.closing())
    {
        double dx = mouse.dx();
        if (dx != 0)
        {
            std::cout << dx << std::endl;
        }
        window.update();
    }
    GameEngine::Engine::terminate();
    return EXIT_SUCCESS;
}
