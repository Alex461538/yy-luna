#include "defines.h"
#include "constants.hpp"

namespace YY
{
    namespace Constants
    {
        namespace Paths
        {
            std::filesystem::path RUNNING_AT;
            std::filesystem::path HOME;

            std::filesystem::path LOG_FILE;
            std::filesystem::path GLOBAL_YYCONF;
        }
        
        #if defined(YY_OSAPI_WIN32)
        const char* OSAPI_NAME = "win32";
        #elif defined(YY_OSAPI_LINUX)
        const char* OSAPI_NAME = "linux";
        #elif defined(YY_OSAPI_DARWIN)
        const char* OSAPI_NAME = "darwin"; // I'll rather die than buying a MAC for testing.
        #elif defined(YY_OSAPI_UNIX)
        const char* OSAPI_NAME = "Unix";
        #endif

        const char *ISA_NAME = YY_ISA;
        const char* VERSION = "0.0.1";
        const char* CLI_BANNER = 
            "╭───────────────────────────────────────────────────────────╮\n" \
            "│ 🌙 Thanks for using yy-luna, your most adrade compiler 🌙 │\n" \
            "╰───────────────────────────────────────────────────────────╯\n";

        void init()
        {
            char exePath[4096];

            #if defined(YY_OSAPI_UNIX)
                ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
                Paths::RUNNING_AT = std::filesystem::path(std::string(&exePath[0], len));
            #elif defined(YY_OSAPI_WIN32)
                // When NULL is passed to GetModuleHandle, the handle of the exe itself is returned
                HMODULE hModule = GetModuleHandle(NULL);
                if (hModule != NULL)
                {
                    GetModuleFileName(hModule, exePath, (sizeof(exePath)));
                    Paths::RUNNING_AT = std::filesystem::path(std::string(exePath));
                }
                else
                {
                    Paths::RUNNING_AT = std::filesystem::path(std::string(""));
                }
            #endif

            // This app is designed to run as portable
            // Env variables are not needed until you want external program/lsp exec
            Paths::HOME = Paths::RUNNING_AT.parent_path() / "../";

            Paths::LOG_FILE = Paths::HOME / "yylog.txt";
            Paths::GLOBAL_YYCONF = Paths::HOME / "yyconf.json";
        }
    }
}