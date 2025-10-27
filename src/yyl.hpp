#include <filesystem>

#include "info.hpp"

namespace YY 
{
    void init()
    {
        YY::PROGRAM_PATH = Info::getProgramPath();
        YY::HOME_PATH = PROGRAM_PATH.parent_path() / "../";
    }
}

/*
Couples to nothing
*/
#include "scramble.hpp"
/*
Couples to Scramble
*/
#include "interp.hpp"

#include "cli.hpp"