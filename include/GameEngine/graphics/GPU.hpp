#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <optional>
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/Engine.hpp>

namespace GameEngine
{
    class Window;

    class GPU
    {

    enum Type {INTEGRATED_GPU, DISCRETE_GPU, VIRTUAL_GPU, CPU, UNKNOWN};

    public:
        GPU() = delete;
        GPU(VkPhysicalDevice device, const Window* window);
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
        static void _destroy_device(VkDevice* logical_device);
    public:
        void operator=(const GPU& other);
    public:
        const std::optional<VkQueue>& _get_graphics_queue() const;
        const std::optional<VkQueue>& _get_transfer_queue() const;
        const std::optional<VkQueue>& _get_compute_queue() const;
        const VkPhysicalDevice& _get_physical_device() const;
        const std::shared_ptr<VkDevice>& _get_logical_device() const;
        const VkPhysicalDeviceProperties& _get_device_properties() const;
        const VkPhysicalDeviceFeatures& _get_device_features() const;
        const VkPhysicalDeviceMemoryProperties& _get_device_memory() const;
    protected:
        std::optional<VkQueue> _graphics_queue;
        std::optional<VkQueue> _transfer_queue;
        std::optional<VkQueue> _compute_queue;
        std::optional<VkQueue> _presentation_queue;
        VkPhysicalDevice _physical_device;
        std::shared_ptr<VkDevice> _logical_device;
        VkPhysicalDeviceProperties _device_properties;
        VkPhysicalDeviceFeatures _device_features;
        VkPhysicalDeviceMemoryProperties _device_memory;
    protected:
        // add a queue family of given type to the selected families
        std::optional<uint32_t> _select_queue_family(std::vector<VkQueueFamilyProperties>& queue_families,
                                                     VkQueueFlagBits queue_type,
                                                     std::map<uint32_t, uint32_t>& selected_families_count) const;
        // select the present queue family specificaly
        std::optional<uint32_t> _select_present_queue_family(std::vector<VkQueueFamilyProperties>& queue_families,
                                                             const Window* window,
                                                             std::map<uint32_t, uint32_t>& selected_families_count) const;
        // query the queue handle of a previously created queue
        void _query_queue_handle(std::optional<VkQueue>& queue,
                                 const std::optional<uint32_t>& queue_family,
                                 std::map<uint32_t, uint32_t>& selected_families_count) const;
    };
}
