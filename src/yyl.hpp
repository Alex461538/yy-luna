#include <filesystem>

#include "info.hpp"
#include "problem.hpp"

namespace YY 
{
    void init()
    {
        YY::PROGRAM_PATH = Info::getProgramPath();
        YY::HOME_PATH = PROGRAM_PATH.parent_path() / "../";
    }
}

#include "config.hpp"
#include "scramble.hpp"
#include "cli.hpp"