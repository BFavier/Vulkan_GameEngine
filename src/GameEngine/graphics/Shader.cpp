#include <GameEngine/graphics/Shader.hpp>
using namespace GameEngine;

std::vector<unsigned char> Shader::_load_binary(const std::string& file_path) const
{
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        THROW_ERROR("failed to open file")
    }
}
