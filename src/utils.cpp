#include "utils.hpp"

namespace YY
{
    namespace Utils
    {
#if defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))

#include <unistd.h>

        std::filesystem::path getProgramPath()
        {
            char exePath[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);

            return std::filesystem::path(std::string(&exePath[0], len));
        }

#elif defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)

#include "windows"

        std::filesystem::path getProgramPath()
        {
            char ownPth[MAX_PATH];

            // When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
            HMODULE hModule = GetModuleHandle(NULL);
            if (hModule != NULL)
            {
                // Use GetModuleFileName() with module handle to get the path
                GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
                return std::filesystem::path(std::string(ownPth));
            }
            else
            {
                return std::filesystem::path(std::string(""));
            }
        }

#endif
    }
}