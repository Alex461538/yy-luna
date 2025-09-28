#include <filesystem>

#include "scramble.hpp"

#include "utils.hpp"

namespace YY 
{
    std::filesystem::path PROGRAM_PATH;

    void init()
    {
        YY::PROGRAM_PATH = Utils::getProgramPath();
    }
}