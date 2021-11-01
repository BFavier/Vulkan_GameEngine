#include <iostream>
#include <GameEngine/game_engine.hpp>
#include <vector>
#include <iostream>
using namespace GameEngine;

int main()
{
    std::cout << "listing available validation layers:" << std::endl;
    std::vector<std::string> layer_names = Engine::get_validation_layers();
    for (std::string& name : layer_names)
    {
        std::cout << "\t" << name << std::endl;
    }
    Engine::initialize({"VK_LAYER_KHRONOS_validation"});
    Window window;
    Mouse& mouse = window.mouse;
    GPU gpu = GPU::get_best_device();
    std::cout << gpu.constructor_name() << " - " << gpu.device_name() << " : " << gpu.memory() << " (" << gpu.type() << ")" << std::endl;
    while(window.is_open())
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
