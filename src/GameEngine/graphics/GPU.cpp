#include <GameEngine/graphics/GPU.hpp>
#include <GameEngine/utilities/Macro.hpp>
#include <set>

using namespace GameEngine;

GPU::GPU(VkPhysicalDevice device)
{
    // device extensions
    const std::vector<const char*> device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    // Save physical device
    _physical_device = device;
    // List properties and features
    vkGetPhysicalDeviceProperties(device, &_device_properties);
    vkGetPhysicalDeviceFeatures(device, &_device_features);
    vkGetPhysicalDeviceMemoryProperties(device, &_device_memory);
    // List the queues
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
    // Select the best matching queue families
    std::map<uint32_t, uint32_t> selected_families_count;
    std::optional<uint32_t> graphics_family = _select_queue_family(queue_families, VK_QUEUE_GRAPHICS_BIT, selected_families_count);
    std::optional<uint32_t> transfer_family = _select_queue_family(queue_families, VK_QUEUE_TRANSFER_BIT, selected_families_count);
    std::optional<uint32_t> compute_family = _select_queue_family(queue_families, VK_QUEUE_COMPUTE_BIT, selected_families_count);
    // Create the queue creation infos
    float priority = 1.;
    std::vector<VkDeviceQueueCreateInfo> selected_families;
    for (std::pair<const uint32_t, uint32_t>& queue_family_count : selected_families_count)
    {
        VkDeviceQueueCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.queueFamilyIndex = queue_family_count.first;
        info.queueCount = queue_family_count.second;
        info.pQueuePriorities = &priority;
        selected_families.push_back(info);
    }
    // Create logical device
    _logical_device.reset(new VkDevice, GPU::_destroy_device);
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = selected_families.data();
    create_info.queueCreateInfoCount = selected_families.size();
    create_info.pEnabledFeatures = &_device_features;
    VkResult result = vkCreateDevice(_physical_device, &create_info, nullptr, _logical_device.get());
    if (result != VK_SUCCESS)
    {
        THROW_ERROR("failed to create logical device")
    }
    // retrieve the queue handles
    _query_queue_handle(_graphics_queue, graphics_family, selected_families_count);
    _query_queue_handle(_transfer_queue, transfer_family, selected_families_count);
    _query_queue_handle(_compute_queue, compute_family, selected_families_count);
}

GPU::GPU(const GPU& other)
{
    _logical_device = other._get_logical_device();
    _physical_device = other._get_physical_device();
    _device_features = other._get_device_features();
    _device_memory = other._get_device_memory();
    _device_properties = other._get_device_properties();
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
            return std::string("UNKNOWN");
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

void GPU::_destroy_device(VkDevice* logical_device)
{
    if (logical_device != nullptr)
    {
        vkDestroyDevice(*logical_device, nullptr);
        delete logical_device;
    }
}

void GPU::operator=(const GPU& other)
{
    _graphics_queue = other._get_graphics_queue();
    _compute_queue = other._get_compute_queue();
    _transfer_queue = other._get_transfer_queue();
    _physical_device = other._get_physical_device();
    _logical_device = other._get_logical_device();
    _device_properties = other._get_device_properties();
    _device_features = other._get_device_features();
    _device_memory = other._get_device_memory();
}

const std::optional<VkQueue>& GPU::_get_graphics_queue() const
{
    return _graphics_queue;
}

const std::optional<VkQueue>& GPU::_get_transfer_queue() const
{
    return _transfer_queue;
}

const std::optional<VkQueue>& GPU::_get_compute_queue() const
{
    return _compute_queue;
}

const VkPhysicalDevice& GPU::_get_physical_device() const
{
    return _physical_device;
}

const std::shared_ptr<VkDevice>& GPU::_get_logical_device() const
{
    return _logical_device;
}

const VkPhysicalDeviceProperties& GPU::_get_device_properties() const
{
    return _device_properties;
}

const VkPhysicalDeviceFeatures& GPU::_get_device_features() const
{
    return _device_features;
}

const VkPhysicalDeviceMemoryProperties& GPU::_get_device_memory() const
{
    return _device_memory;
}

std::optional<uint32_t> GPU::_select_queue_family(std::vector<VkQueueFamilyProperties>& queue_families,
                                                  VkQueueFlagBits queue_type,
                                                  std::map<uint32_t, uint32_t>& selected_families_count) const
{
    std::vector<VkQueueFlagBits> functionalities = {VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_TRANSFER_BIT};
    // filter families that handle the operation type
    std::map<uint32_t, VkQueueFamilyProperties> valid_families;
    for (uint32_t i=0; i<queue_families.size(); i++)
    {
        VkQueueFamilyProperties& queue_family = queue_families[i];
        if ((queue_family.queueFlags & queue_type) &&
            (queue_family.queueCount > 0))
        {
            valid_families[i] = queue_family;
        }
    }
    // Looking for the most specialized queue
    std::optional<uint32_t> selected_family;
    unsigned int min_n_functionalities;
    for (std::pair<const uint32_t, VkQueueFamilyProperties>& family : valid_families)
    {
        // if there is no selected family yet, set current family and number of functionalities
        if (!selected_family.has_value())
        {
            selected_family = family.first;
            VkQueueFlags flags = family.second.queueFlags;
            min_n_functionalities = 0;
            for (VkQueueFlagBits bits : functionalities)
            {
                min_n_functionalities += (flags & bits) / bits;
            }
        }
        // otherwise change selected family only if it has less functionalities
        else
        {
            VkQueueFlags flags = family.second.queueFlags;
            unsigned int n_functionalities = 0;
            for (VkQueueFlagBits bits : functionalities)
            {
                n_functionalities += (flags & bits) / bits;
            }
            if (n_functionalities < min_n_functionalities)
            {
                selected_family = family.first;
            }
        }
    }
    // if a family was selected decrement its number of available queues, and increment its selected count
    if (selected_family.has_value())
    {
        queue_families[selected_family.value()].queueCount -= 1;
        if (selected_families_count.find(selected_family.value()) != selected_families_count.end())
        {
            selected_families_count[selected_family.value()] += 1;
        }
        else
        {
            selected_families_count[selected_family.value()] = 1;
        }
    }
    // return the selected family if there was any
    return selected_family;
}


void GPU::_query_queue_handle(std::optional<VkQueue>& queue,
                              const std::optional<uint32_t>& queue_family,
                              std::map<uint32_t, uint32_t>& selected_families_count) const
{
    if (queue_family.has_value())
    {
        uint32_t family = queue_family.value();
        VkQueue queried_queue;
        vkGetDeviceQueue(*_logical_device, family, selected_families_count[family]-1, &queried_queue);
        queue = queried_queue;
        selected_families_count[family] -= 1;
    }
}