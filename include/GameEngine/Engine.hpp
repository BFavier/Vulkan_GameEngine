#pragma once
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/utilities/Macro.hpp>

namespace GameEngine
{

    class Engine
    {
    public:
        //Initialize the used libraries
        static void initialize();
        static void terminate();
        static VkInstance get_vulkan_instance();
    protected:
        ///< If true, the game engine was already initialized
        static bool _initialized;
        static VkInstance _vk_instance;
    };
}