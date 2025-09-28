#include "info.hpp"

namespace YY
{
    namespace Info
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

#if defined(_WIN32)
        const char *OS_NAME = "windows";
#elif defined(__linux__)
        const char *OS_NAME = "linux";
#elif defined(__APPLE__)
        const char *OS_NAME = "macos";
#else
        const char *OS_NAME = "unknown";
#endif

#if defined(__x86_64__) || defined(_M_X64)
        const char *ARCH_NAME = "x64";
#elif defined(__i386__) || defined(_M_IX86)
        const char *ARCH_NAME = "x86";
#elif defined(__aarch64__)
        const char *ARCH_NAME = "arm64";
#elif defined(__arm__)
        const char *ARCH_NAME = "arm";
#else
        const char *ARCH_NAME = "unknown";
#endif
    }
}