#pragma once
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/utilities/Macro.hpp>
#include <vector>

namespace GameEngine
{

    class Engine
    {
    public:
        //Initialize the used libraries
        static void initialize(const std::vector<std::string>& validation_layers={});
        static void terminate();
        static std::vector<std::string> get_validation_layers();
        static VkInstance get_vulkan_instance();
    protected:
        ///< If true, the game engine was already initialized
        static bool _initialized;
        static VkInstance _vk_instance;
    };
}