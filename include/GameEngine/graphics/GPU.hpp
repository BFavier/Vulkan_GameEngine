#pragma once
#include <vector>
#include <string>
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/Engine.hpp>

namespace GameEngine
{
    class GPU
    {
    
    enum Type {INTEGRATED_GPU, DISCRETE_GPU, VIRTUAL_GPU, CPU, UNKNOWN};
    
    public:
        GPU() = delete;
        GPU(VkPhysicalDevice device);
        GPU(const GPU& other);
        ~GPU();
        // Device name
        std::string device_name() const;
        // Device constructor name
        std::string constructor_name() const;
        // Device total local memory in bytes
        unsigned int memory() const;
        // Returns the type of the device
        Type type() const;
    public:
        static std::vector<GPU> get_devices();
        static GPU get_best_device();
    public:
        VkPhysicalDeviceProperties _get_device_propeties() const;
        VkPhysicalDeviceFeatures _get_device_features() const;
        VkPhysicalDeviceMemoryProperties _get_device_memory() const;
    protected:
        VkPhysicalDeviceProperties _device_properties;
        VkPhysicalDeviceFeatures _device_features;
        VkPhysicalDeviceMemoryProperties _device_memory;
    };
}
