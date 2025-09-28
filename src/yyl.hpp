#include <filesystem>

#include "info.hpp"

namespace YY 
{
    std::filesystem::path PROGRAM_PATH;

    void init()
    {
        YY::PROGRAM_PATH = Info::getProgramPath();
    }
}

#include "scramble.hpp"
#include "cli.hpp"