#include <GameEngine/Engine.hpp>

using namespace GameEngine;

bool Engine::_initialized = false;
VkInstance Engine::_vk_instance;

void Engine::initialize(bool debug)
{
    if (_initialized)
    {
        return;
    }
    //Initialize GLFW
    if (!glfwInit())
    {
        THROW_ERROR("Failed to initialize the library GLFW")
    }
    //Initialize Vulkan
    std::vector<const char*> validation_layers;
    if (debug)
    {
        validation_layers.push_back("VK_LAYER_KHRONOS_validation");
    }
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GameEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.ppEnabledLayerNames = validation_layers.data();
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &_vk_instance);
    if (result != VK_SUCCESS)
    {
        THROW_ERROR("Failed to create the Vulkan instance")
    }
}

void Engine::terminate()
{
    //Terminate Vulkan
    vkDestroyInstance(_vk_instance, nullptr);
    //Terminate GLFW
    glfwTerminate();
}

VkInstance Engine::get_vulkan_instance()
{
    Engine::initialize();
    return _vk_instance;
}