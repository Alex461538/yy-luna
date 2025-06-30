#include "glin/paths.hpp"

#include <string.h>
#include <filesystem>

namespace Paths {
    std::string getGlinHome()
    {
        const char* home = std::getenv("GLIN_HOME");
        return home ? std::string(home) : "";
    }

    std::string getTrueCallerPath(std::string directory)
    {
        std::filesystem::path other = directory;
        if (other.is_absolute())
        {
            return other.string();
        }
        return (std::filesystem::current_path() / other).string();
    }
}