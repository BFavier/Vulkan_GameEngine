#include <GameEngine/Engine.hpp>

using namespace GameEngine;

bool Engine::_initialized = false;
VkInstance Engine::_vk_instance;
VkDebugUtilsMessengerEXT Engine::_debug_messenger;

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
    // Set application name
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GameEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    // setup validation layers
    std::vector<std::string> available_validation_layers = get_available_validation_layers();
    std::vector<const char*> validation_layer_names;
    for (const std::string& layer_name : validation_layers)
    {
        if (std::find(available_validation_layers.begin(), available_validation_layers.end(), layer_name) == available_validation_layers.end())
        {
            std::string error_message = "Unsupported validation layer: '" + layer_name + "'";
            THROW_ERROR(error_message)
        }
        validation_layer_names.push_back(layer_name.c_str());
    }
    // Initialize Vulkan
    std::vector<const char*> extensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = validation_layer_names.size();
    createInfo.ppEnabledLayerNames = validation_layer_names.data();
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint32_t i=0; i<glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    VkResult result = vkCreateInstance(&createInfo, nullptr, &_vk_instance);
    if (result != VK_SUCCESS)
    {
        THROW_ERROR("Failed to create the Vulkan instance")
    }
    // setup validation layers callback function
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = _debug_callback;
    debugInfo.pUserData = nullptr;
    if (_create_debug_utils_messenger_EXT(_vk_instance, &debugInfo, nullptr, &_debug_messenger) != VK_SUCCESS)
    {
        THROW_ERROR("Failed to set up debug messenger")
    }
    // setup the terminate function
    std::atexit(Engine::terminate);
}

void Engine::terminate()
{
    //Terminate Vulkan
    _destroy_debug_utils_messenger_EXT(_vk_instance, _debug_messenger, nullptr);
    vkDestroyInstance(_vk_instance, nullptr);
    //Terminate GLFW
    glfwTerminate();
}

std::vector<std::string> Engine::get_available_validation_layers()
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

std::vector<std::string> Engine::get_available_vulkan_extensions()
{
    uint32_t extension_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
    std::vector<std::string> available_extensions;
    for (VkExtensionProperties& extension : extensions)
    {
        available_extensions.push_back(std::string(extension.extensionName));
    }
    return available_extensions;
}

VkInstance Engine::get_vulkan_instance()
{
    Engine::initialize();
    return _vk_instance;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Engine::_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                       void* pUserData)
{
    pCallbackData;
    pUserData;
    messageType;
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "[validation layer]" << std::endl;
        std::cerr << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}

VkResult Engine::_create_debug_utils_messenger_EXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Engine::_destroy_debug_utils_messenger_EXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}