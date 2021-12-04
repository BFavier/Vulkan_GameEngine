#pragma once
#include <GameEngine/utilities/Macro.hpp>
#include <vector>
#include <string>
#include <fstream>

namespace GameEngine
{
    class Shader
    {
    public:
        Shader() = delete;
        ~Shader();
    protected:
        std::vector<unsigned char> _load_binary(const std::string& file_path) const;
    };
}
