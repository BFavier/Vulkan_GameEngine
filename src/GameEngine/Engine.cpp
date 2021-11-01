#include <GameEngine/Engine.hpp>

using namespace GameEngine;

bool Engine::_initialized = false;
VkInstance Engine::_vk_instance;

void Engine::initialize(const std::vector<std::string>& validation_layers)
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
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GameEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    std::vector<const char*> validation_layer_names;
    for (const std::string& layer_name : validation_layers)
    {
        validation_layer_names.push_back(layer_name.c_str());
    }
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = validation_layer_names.size();
    createInfo.ppEnabledLayerNames = validation_layer_names.data();
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

std::vector<std::string> Engine::get_validation_layers()
{
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());
    std::vector<std::string> layer_names;
    for (VkLayerProperties layer : available_layers)
    {
        layer_names.push_back(std::string(layer.layerName));
    }
    return layer_names;
}

VkInstance Engine::get_vulkan_instance()
{
    Engine::initialize();
    return _vk_instance;
}