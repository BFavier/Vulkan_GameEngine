# pragma once
#include <GameEngine/utilities/External.hpp>
#include <GameEngine/graphics/GPU.hpp>
#include <string>

namespace GameEngine
{
    class ImageView;

    class Image
    {
    public:
        enum Format {GRAY, RGB, RGBA};
    public:
        Image() = delete;
        Image(const GPU& gpu, const std::string& file_path);
        Image(const GPU& gpu, unsigned int width, unsigned int height, Format format, const std::vector<unsigned char>& data);
        ~Image();
    public:
        const GPU& gpu;
        unsigned int width;
        unsigned int height;
        Format _format;
        VkFormat _vk_image_format;
        VkImage _vk_image;
    };
}