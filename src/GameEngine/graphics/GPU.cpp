#include <GameEngine/graphics/GPU.hpp>
#include <GameEngine/utilities/Macro.hpp>

using namespace GameEngine;

GPU::GPU(VkPhysicalDevice device)
{
    vkGetPhysicalDeviceProperties(device, &_device_properties);
    vkGetPhysicalDeviceFeatures(device, &_device_features);
    vkGetPhysicalDeviceMemoryProperties(device, &_device_memory);
}

GPU::GPU(const GPU& other)
{
    _device_features = other._get_device_features();
    _device_memory = other._get_device_memory();
    _device_properties = other._get_device_propeties();
}

GPU::~GPU()
{
}

std::string GPU::device_name() const
{
    return std::string(_device_properties.deviceName);
}

std::string GPU::constructor_name() const
{
    switch (_device_properties.vendorID)
    {
        case 0x1002:
            return std::string("AMD");
        case 0x1010:
            return std::string("ImgTec");
        case 0x10DE:
            return std::string("NVIDIA");
        case 0x13B5:
            return std::string("ARM");
        case 0x5143:
            return std::string("Qualcomm");
        case 0x8086:
            return std::string("Intel");
        default:
            return std::string("UNKNOWN_CONSTRUCTOR");
    }
}

unsigned int GPU::memory() const
{
    uint32_t n_heaps = _device_memory.memoryHeapCount;
    unsigned int memory = 0;
    for (uint32_t i=0; i<n_heaps; i++)
    {
        VkMemoryHeap heap = _device_memory.memoryHeaps[i];
        if (heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            memory += static_cast<unsigned int>(heap.size);
        }
    }
    return memory;
}

GPU::Type GPU::type() const
{
    switch (_device_properties.deviceType)
    {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return INTEGRATED_GPU;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return DISCRETE_GPU;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return VIRTUAL_GPU;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return CPU;
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return UNKNOWN;
        default:
            return UNKNOWN;
    }
}

std::vector<GPU> GPU::get_devices()
{
    VkInstance instance = Engine::get_vulkan_instance();
    // Get the number of devices
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    // Get all the devices
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());
    // Return the GPU objects
    std::vector<GPU> GPUs;
    for(VkPhysicalDevice& device : devices)
    {
        GPUs.push_back(device);
    }
    return GPUs;
}

GPU GPU::get_best_device()
{
    // list the available GPUs and split them by type
    std::vector<GPU> GPUs = get_devices();
    std::vector<GPU> discrete_GPUs;
    std::vector<GPU> other_GPUs;
    for (GPU& gpu : GPUs)
    {
        if (gpu.type() == GPU::Type::DISCRETE_GPU)
        {
            discrete_GPUs.push_back(gpu);
        }
        else
        {
            other_GPUs.push_back(gpu);
        }
    }
    // chose the best available subset of GPUs
    std::vector<GPU> subset;
    if (discrete_GPUs.size() > 0)
    {
        subset = discrete_GPUs;
    }
    else if (other_GPUs.size() > 0)
    {
        subset = other_GPUs;
    }
    else
    {
        THROW_ERROR("No GPU found that match the criteria")
    }
    // select the GPU with most memory
    unsigned int max_memory = 0;
    unsigned int i_max = 0;
    for (unsigned int i=0; i<subset.size(); i++)
    {
        GPU gpu = subset[i];
        unsigned int memory = gpu.memory();
        if (memory > max_memory)
        {
            i_max = i;
            max_memory = memory;
        }
    }
    return subset[i_max];
}

VkPhysicalDeviceProperties GPU::_get_device_propeties() const
{
    return _device_properties;
}

VkPhysicalDeviceFeatures GPU::_get_device_features() const
{
    return _device_features;
}

VkPhysicalDeviceMemoryProperties GPU::_get_device_memory() const
{
    return _device_memory;
}
