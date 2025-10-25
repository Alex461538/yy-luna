#if !defined(__hpp_yy_utils__)
#define __hpp_yy_utils__

#include <filesystem>
#include <string>
#include <limits.h>

namespace YY
{
    extern std::filesystem::path PROGRAM_PATH;
    extern std::filesystem::path HOME_PATH;

    namespace Info
    {
        std::filesystem::path getProgramPath();

        extern const char *OS_NAME;
        extern const char *ARCH_NAME;
    }
}

#endif